#include "imgprmt_jpeg.hpp"

#include <turbojpeg.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <format>
#include <optional>
#include <ranges>
#include <stdexcept>

namespace {
using Byte = std::uint8_t;

[[nodiscard]] std::optional<std::size_t> searchSig(std::span<const Byte> v, std::span<const Byte> sig, std::size_t limit = 0) {
	auto search_span = (limit == 0 || limit > v.size()) ? v : v.first(limit);
	auto it = std::ranges::search(search_span, sig);
	if (it.empty()) {
		return std::nullopt;
	}
	return static_cast<std::size_t>(it.begin() - v.begin());
}

int getTransformOp(std::uint16_t orientation) {
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

struct TJHandle {
	tjhandle handle = nullptr;

	TJHandle() = default;

	TJHandle(const TJHandle&) = delete;
	TJHandle& operator=(const TJHandle&) = delete;

	TJHandle(TJHandle&& other) noexcept : handle(other.handle) {
		other.handle = nullptr;
	}

	TJHandle& operator=(TJHandle&& other) noexcept {
		if (this != &other) {
			reset();
			handle = other.handle;
			other.handle = nullptr;
		}
		return *this;
	}

	~TJHandle() {
		reset();
	}

	void reset() {
		if (handle) {
			tjDestroy(handle);
			handle = nullptr;
		}
	}

	tjhandle get() const {
		return handle;
	}

	explicit operator bool() const {
		return handle != nullptr;
	}
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
std::optional<std::uint16_t> exifOrientation(std::span<const Byte> jpg) {
	constexpr std::size_t EXIF_SEARCH_LIMIT = 4096;
	constexpr auto APP1_SIG = std::to_array<Byte>({0xFF, 0xE1});

	auto app1_pos_opt = searchSig(jpg, APP1_SIG, EXIF_SEARCH_LIMIT);
	if (!app1_pos_opt) {
		return std::nullopt;
	}

	std::size_t pos = *app1_pos_opt;
	if (pos + 4 > jpg.size()) {
		return std::nullopt;
	}

	const std::size_t segment_length =
		(static_cast<std::size_t>(jpg[pos + 2]) << 8) | static_cast<std::size_t>(jpg[pos + 3]);
	if (segment_length < 2) {
		return std::nullopt;
	}

	const std::size_t exif_end = pos + 2 + segment_length;
	if (exif_end > jpg.size()) {
		return std::nullopt;
	}

	std::span<const Byte> payload(jpg.data() + pos + 4, segment_length - 2);

	constexpr std::size_t EXIF_HEADER_SIZE = 6;
	constexpr auto EXIF_SIG = std::to_array<Byte>({'E', 'x', 'i', 'f', '\0', '\0'});
	if (payload.size() < EXIF_HEADER_SIZE || !std::ranges::equal(payload.first(EXIF_HEADER_SIZE), EXIF_SIG)) {
		return std::nullopt;
	}

	std::span<const Byte> tiff_data = payload.subspan(EXIF_HEADER_SIZE);
	if (tiff_data.size() < 8) {
		return std::nullopt;
	}

	bool is_le = false;
	if (tiff_data[0] == 'I' && tiff_data[1] == 'I') {
		is_le = true;
	} else if (tiff_data[0] == 'M' && tiff_data[1] == 'M') {
		is_le = false;
	} else {
		return std::nullopt;
	}

	auto read16 = [&](std::size_t offset) -> std::uint16_t {
		if (offset + 2 > tiff_data.size()) {
			return 0;
		}
		std::uint16_t value;
		std::memcpy(&value, tiff_data.data() + offset, 2);
		return is_le ? value : std::byteswap(value);
	};

	auto read32 = [&](std::size_t offset) -> std::uint32_t {
		if (offset + 4 > tiff_data.size()) {
			return 0;
		}
		std::uint32_t value;
		std::memcpy(&value, tiff_data.data() + offset, 4);
		return is_le ? value : std::byteswap(value);
	};

	if (read16(2) != 0x002A) {
		return std::nullopt;
	}

	const std::uint32_t ifd_offset = read32(4);
	if (ifd_offset < 8 || ifd_offset >= tiff_data.size()) {
		return std::nullopt;
	}

	const std::uint16_t entry_count = read16(ifd_offset);
	std::size_t current_entry = ifd_offset + 2;

	constexpr std::uint16_t TAG_ORIENTATION = 0x0112;
	constexpr std::size_t ENTRY_SIZE = 12;
	for (std::uint16_t i = 0; i < entry_count; ++i) {
		if (current_entry + ENTRY_SIZE > tiff_data.size()) {
			return std::nullopt;
		}
		const std::uint16_t tag_id = read16(current_entry);
		if (tag_id == TAG_ORIENTATION) {
			return read16(current_entry + 8);
		}
		current_entry += ENTRY_SIZE;
	}
	return std::nullopt;
}

int estimateImageQuality(std::span<const Byte> jpg) {
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

	constexpr auto DQT_SIG = std::to_array<Byte>({0xFF, 0xDB});
	constexpr std::size_t DQT_SEARCH_LIMIT = 32768;

	auto dqt_pos_opt = searchSig(jpg, DQT_SIG, DQT_SEARCH_LIMIT);
	if (!dqt_pos_opt) {
		return 80;
	}

	std::size_t pos = *dqt_pos_opt;
	if (pos + 4 > jpg.size()) {
		return 80;
	}

	std::size_t length = (static_cast<std::size_t>(jpg[pos + 2]) << 8) | jpg[pos + 3];
	const std::size_t end = pos + 2 + length;
	if (end > jpg.size()) {
		return 80;
	}

	pos += 4;
	while (pos < end) {
		const Byte header = jpg[pos++];
		const Byte precision = (header >> 4) & 0x0F;
		const Byte table_id = header & 0x0F;

		const std::size_t table_size = (precision == 0) ? 64 : 128;
		if (pos + table_size > end) {
			break;
		}

		if (table_id == 0) {
			const int sum = std::ranges::fold_left(std::views::iota(0uz, 64uz), 0, [&](int acc, std::size_t i) {
				return acc + (precision == 0 ? jpg[pos + i]
				                             : (static_cast<int>(jpg[pos + i * 2]) << 8) | jpg[pos + i * 2 + 1]);
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

void optimizeImage(vBytes& jpg_vec) {
	if (jpg_vec.empty()) {
		throw std::runtime_error("JPG image is empty!");
	}

	TJHandle transformer;
	transformer.handle = tjInitTransform();
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

	if (width < 400 || height < 400) {
		throw std::runtime_error("Image Error: Dimensions are too small.\nFor platform compatibility, cover image must be at least 400px for both width and height.");
	}

	auto ori_opt = exifOrientation(jpg_vec);
	int xop = TJXOP_NONE;
	if (ori_opt) {
		xop = getTransformOp(*ori_opt);
	}

	tjtransform xform;
	std::memset(&xform, 0, sizeof(tjtransform));
	xform.op = xop;
	xform.options = TJXOPT_COPYNONE | TJXOPT_TRIM | TJXOPT_PROGRESSIVE;

	TJBuffer dstBuffer;
	unsigned long dstSize = 0;

	if (tjTransform(transformer.get(), jpg_vec.data(), static_cast<unsigned long>(jpg_vec.size()), 1, &dstBuffer.data, &dstSize, &xform, 0) != 0) {
		throw std::runtime_error(std::format("tjTransform: {}", tjGetErrorStr2(transformer.get())));
	}

	jpg_vec.assign(dstBuffer.data, dstBuffer.data + dstSize);

	const int estimated_quality = estimateImageQuality(jpg_vec);
	if (estimated_quality > 97) {
		throw std::runtime_error("Image Error: Quality too high. For platform compatibility, cover image quality must be 97 or lower.");
	}
}
}  // namespace imgprmt::jpeg
