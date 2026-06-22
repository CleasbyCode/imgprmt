#include "imgprmt_jpeg.hpp"

#include <turbojpeg.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <format>
#include <optional>
#include <ranges>
#include <stdexcept>

namespace {
using Byte = std::uint8_t;

constexpr int MIN_IMAGE_DIMENSION = 400;
constexpr int MAX_IMAGE_DIMENSION = 16384;
constexpr std::int64_t MAX_IMAGE_PIXELS = 100'000'000;

struct JpegSegment {
	Byte marker = 0;
	std::size_t marker_pos = 0;
	std::span<const Byte> payload;
};

[[nodiscard]] bool is_standalone_marker(Byte marker) noexcept {
	return marker == 0x00 || marker == 0x01 || (marker >= 0xD0 && marker <= 0xD7);
}

[[nodiscard]] std::optional<JpegSegment> next_jpeg_segment(std::span<const Byte> jpg, std::size_t& pos, std::size_t limit) {
	if (jpg.size() < 2 || jpg[0] != 0xFF || jpg[1] != 0xD8) {
		return std::nullopt;
	}

	limit = (limit == 0 || limit > jpg.size()) ? jpg.size() : limit;
	while (pos < limit) {
		const void* match = std::memchr(jpg.data() + pos, 0xFF, limit - pos);
		if (!match) {
			return std::nullopt;
		}
		const std::size_t marker_pos = static_cast<const Byte*>(match) - jpg.data();
		pos = marker_pos;

		while (pos < limit && jpg[pos] == 0xFF) {
			++pos;
		}
		if (pos >= limit) {
			return std::nullopt;
		}

		const Byte marker = jpg[pos++];
		if (marker == 0xDA || marker == 0xD9) {
			return std::nullopt;
		}
		if (is_standalone_marker(marker)) {
			continue;
		}
		if (jpg.size() - pos < 2) {
			return std::nullopt;
		}

		const std::size_t length = (static_cast<std::size_t>(jpg[pos]) << 8) |
		                           static_cast<std::size_t>(jpg[pos + 1]);
		if (length < 2 || length > jpg.size() - pos) {
			return std::nullopt;
		}

		const auto payload = std::span<const Byte>(jpg.data() + pos + 2, length - 2);
		pos += length;
		return JpegSegment{
			.marker = marker,
			.marker_pos = marker_pos,
			.payload = payload,
		};
	}

	return std::nullopt;
}

[[nodiscard]] std::optional<JpegSegment> find_jpeg_segment(std::span<const Byte> jpg, Byte marker, std::size_t limit = 0) {
	std::size_t pos = 2;
	while (auto segment = next_jpeg_segment(jpg, pos, limit)) {
		if (segment->marker == marker) {
			return segment;
		}
	}
	return std::nullopt;
}

[[nodiscard]] int get_transform_op(std::uint16_t orientation) noexcept {
	switch (orientation) {
		case 2:
			return TJXOP_HFLIP;
		case 3:
			return TJXOP_ROT180;
		case 4:
			return TJXOP_VFLIP;
		case 5:
			return TJXOP_TRANSPOSE;
		case 6:
			return TJXOP_ROT90;
		case 7:
			return TJXOP_TRANSVERSE;
		case 8:
			return TJXOP_ROT270;
		default:
			return TJXOP_NONE;
	}
}

struct TiffReader {
	std::span<const Byte> data;
	bool is_little_endian = false;

	[[nodiscard]] std::uint16_t read16(std::size_t offset) const {
		if (offset > data.size() || data.size() - offset < 2) {
			return 0;
		}
		std::uint16_t value;
		std::memcpy(&value, data.data() + offset, 2);
		return is_little_endian ? value : std::byteswap(value);
	}

	[[nodiscard]] std::uint32_t read32(std::size_t offset) const {
		if (offset > data.size() || data.size() - offset < 4) {
			return 0;
		}
		std::uint32_t value;
		std::memcpy(&value, data.data() + offset, 4);
		return is_little_endian ? value : std::byteswap(value);
	}
};

[[nodiscard]] std::optional<TiffReader> make_tiff_reader(std::span<const Byte> tiff_data) {
	if (tiff_data.size() < 8) {
		return std::nullopt;
	}

	if (tiff_data[0] == 'I' && tiff_data[1] == 'I') {
		return TiffReader{.data = tiff_data, .is_little_endian = true};
	}
	if (tiff_data[0] == 'M' && tiff_data[1] == 'M') {
		return TiffReader{.data = tiff_data, .is_little_endian = false};
	}
	return std::nullopt;
}

void validate_image_dimensions(int width, int height) {
	if (width <= 0 || height <= 0) {
		throw std::runtime_error("Image Error: Invalid image dimensions.");
	}
	if (width < MIN_IMAGE_DIMENSION || height < MIN_IMAGE_DIMENSION) {
		throw std::runtime_error("Image Error: Dimensions are too small.\nFor platform compatibility, cover image must be at least 400px for both width and height.");
	}
	if (width > MAX_IMAGE_DIMENSION || height > MAX_IMAGE_DIMENSION ||
	    static_cast<std::int64_t>(width) * static_cast<std::int64_t>(height) > MAX_IMAGE_PIXELS) {
		throw std::runtime_error("Image Error: Dimensions are too large.");
	}
}

class TJHandle {
public:
	explicit TJHandle(tjhandle h) noexcept : handle_(h) {}

	TJHandle(const TJHandle&) = delete;
	TJHandle& operator=(const TJHandle&) = delete;
	TJHandle(TJHandle&&) = delete;
	TJHandle& operator=(TJHandle&&) = delete;

	~TJHandle() {
		if (handle_) {
			tjDestroy(handle_);
		}
	}

	tjhandle get() const noexcept { return handle_; }
	explicit operator bool() const noexcept { return handle_ != nullptr; }

private:
	tjhandle handle_ = nullptr;
};

struct TJBuffer {
	unsigned char* data = nullptr;

	TJBuffer() = default;
	~TJBuffer() {
		if (data) {
			tjFree(data);
		}
	}

	TJBuffer(const TJBuffer&) = delete;
	TJBuffer& operator=(const TJBuffer&) = delete;
	TJBuffer(TJBuffer&&) = delete;
	TJBuffer& operator=(TJBuffer&&) = delete;
};
}  // namespace

namespace imgprmt::jpeg {
std::optional<std::size_t> find_marker_offset(std::span<const Byte> jpg, Byte marker, std::size_t limit) {
	auto segment = find_jpeg_segment(jpg, marker, limit);
	if (!segment) {
		return std::nullopt;
	}
	return segment->marker_pos;
}

std::optional<std::uint16_t> exif_orientation(std::span<const Byte> jpg) {
	constexpr std::size_t EXIF_SEARCH_LIMIT = 4096;
	constexpr Byte APP1_MARKER = 0xE1;

	auto app1_segment_opt = find_jpeg_segment(jpg, APP1_MARKER, EXIF_SEARCH_LIMIT);
	if (!app1_segment_opt) {
		return std::nullopt;
	}

	std::span<const Byte> payload = app1_segment_opt->payload;

	constexpr std::size_t EXIF_HEADER_SIZE = 6;
	constexpr auto EXIF_SIG = std::to_array<Byte>({'E', 'x', 'i', 'f', '\0', '\0'});
	if (payload.size() < EXIF_HEADER_SIZE || !std::ranges::equal(payload.first(EXIF_HEADER_SIZE), EXIF_SIG)) {
		return std::nullopt;
	}

	std::span<const Byte> tiff_data = payload.subspan(EXIF_HEADER_SIZE);
	auto tiff_reader_opt = make_tiff_reader(tiff_data);
	if (!tiff_reader_opt) {
		return std::nullopt;
	}
	const TiffReader tiff = *tiff_reader_opt;

	if (tiff.read16(2) != 0x002A) {
		return std::nullopt;
	}

	const std::uint32_t ifd_offset = tiff.read32(4);
	if (ifd_offset < 8 || ifd_offset > tiff_data.size() - 2) {
		return std::nullopt;
	}

	const std::uint16_t entry_count = tiff.read16(ifd_offset);
	std::size_t current_entry = ifd_offset + 2;

	constexpr std::uint16_t TAG_ORIENTATION = 0x0112;
	constexpr std::size_t ENTRY_SIZE = 12;
	for (std::uint16_t i = 0; i < entry_count; ++i) {
		if (current_entry > tiff_data.size() || tiff_data.size() - current_entry < ENTRY_SIZE) {
			return std::nullopt;
		}
		const std::uint16_t tag_id = tiff.read16(current_entry);
		if (tag_id == TAG_ORIENTATION) {
			return tiff.read16(current_entry + 8);
		}
		current_entry += ENTRY_SIZE;
	}
	return std::nullopt;
}

int estimate_image_quality(std::span<const Byte> jpg) {
	constexpr std::array<int, 101> STD_LUMINANCE_SUMS = {
		0,
		16320, 16315, 15946, 15277, 14655, 14073, 13623, 13230, 12859, 12560,
		12240, 11861, 11456, 11081, 10714, 10360, 10027, 9679, 9368, 9056,
		8680, 8331, 7995, 7668, 7376, 7084, 6823, 6562, 6345, 6125,
		5939, 5756, 5571, 5421, 5240, 5086, 4976, 4829, 4719, 4616,
		4463, 4393, 4280, 4166, 4092, 3980, 3909, 3835, 3755, 3688,
		3621, 3541, 3467, 3396, 3323, 3247, 3170, 3096, 3021, 2952,
		2874, 2804, 2727, 2657, 2583, 2509, 2437, 2362, 2290, 2211,
		2136, 2068, 1996, 1915, 1858, 1773, 1692, 1620, 1552, 1477,
		1398, 1326, 1251, 1179, 1109, 1031, 961, 884, 814, 736,
		667, 592, 518, 441, 369, 292, 221, 151, 86, 64};

	constexpr Byte DQT_MARKER = 0xDB;
	constexpr std::size_t DQT_SEARCH_LIMIT = 32768;

	auto dqt_segment_opt = find_jpeg_segment(jpg, DQT_MARKER, DQT_SEARCH_LIMIT);
	if (!dqt_segment_opt) {
		return 80;
	}

	std::span<const Byte> dqt_payload = dqt_segment_opt->payload;
	std::size_t pos = 0;
	const std::size_t end = dqt_payload.size();
	while (pos < end) {
		const Byte header = dqt_payload[pos++];
		const Byte precision = (header >> 4) & 0x0F;
		const Byte table_id = header & 0x0F;

		const std::size_t table_size = (precision == 0) ? 64 : 128;
		if (table_size > end - pos) {
			break;
		}

		if (table_id == 0) {
			const int sum = std::ranges::fold_left(std::views::iota(0uz, 64uz), 0, [&](int acc, std::size_t i) {
				return acc + (precision == 0 ? dqt_payload[pos + i]
				                             : (static_cast<int>(dqt_payload[pos + i * 2]) << 8) | dqt_payload[pos + i * 2 + 1]);
			});
			if (sum <= 64) {
				return 100;
			}
			if (sum >= 16320) {
				return 1;
			}

			for (std::size_t q = 1; q < STD_LUMINANCE_SUMS.size(); ++q) {
				if (sum >= STD_LUMINANCE_SUMS[q]) {
					if (q > 1) {
						const int diff_current = sum - STD_LUMINANCE_SUMS[q];
						const int diff_prev = STD_LUMINANCE_SUMS[q - 1] - sum;
						if (diff_prev < diff_current) {
							return static_cast<int>(q - 1);
						}
					}
					return static_cast<int>(q);
				}
			}
			return 100;
		}
		pos += table_size;
	}
	return 80;
}

void optimize_image(vBytes& jpg_vec) {
	if (jpg_vec.empty()) {
		throw std::runtime_error("JPG image is empty!");
	}

	TJHandle transformer{tjInitTransform()};
	if (!transformer) {
		throw std::runtime_error("tjInitTransform() failed");
	}

	int width = 0;
	int height = 0;
	int jpegSubsamp = 0;
	int jpegColorspace = 0;
	if (tjDecompressHeader3(transformer.get(), jpg_vec.data(), static_cast<unsigned long>(jpg_vec.size()), &width, &height, &jpegSubsamp, &jpegColorspace) != 0) {
		throw std::runtime_error(std::format("Image Error: {}", tjGetErrorStr2(transformer.get())));
	}

	validate_image_dimensions(width, height);

	auto ori_opt = exif_orientation(jpg_vec);
	int xop = TJXOP_NONE;
	if (ori_opt) {
		xop = get_transform_op(*ori_opt);
	}

	tjtransform xform{};
	xform.op = xop;
	xform.options = TJXOPT_COPYNONE | TJXOPT_TRIM | TJXOPT_PROGRESSIVE;

	TJBuffer dstBuffer;
	unsigned long dstSize = 0;

	if (tjTransform(transformer.get(), jpg_vec.data(), static_cast<unsigned long>(jpg_vec.size()), 1, &dstBuffer.data, &dstSize, &xform, 0) != 0) {
		throw std::runtime_error(std::format("tjTransform: {}", tjGetErrorStr2(transformer.get())));
	}
	if (!dstBuffer.data || dstSize == 0) {
		throw std::runtime_error("tjTransform: Empty output image.");
	}
	if (dstSize > std::numeric_limits<std::size_t>::max()) {
		throw std::runtime_error("tjTransform: Output image size exceeds supported limits.");
	}

	const auto output_size = static_cast<std::size_t>(dstSize);
	jpg_vec.assign(dstBuffer.data, dstBuffer.data + output_size);

	const int estimated_quality = estimate_image_quality(jpg_vec);
	if (estimated_quality > 97) {
		throw std::runtime_error("Image Error: Quality too high. For platform compatibility, cover image quality must be 97 or lower.");
	}
}
}  // namespace imgprmt::jpeg
