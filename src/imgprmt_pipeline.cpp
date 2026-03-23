#include "imgprmt_pipeline.hpp"

#include "imgprmt_io.hpp"
#include "imgprmt_jpeg.hpp"
#include "imgprmt_text.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstring>
#include <format>
#include <limits>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <utility>
#include <sys/stat.h>
#include <unistd.h>

namespace imgprmt::pipeline {
namespace {
[[nodiscard]] std::optional<std::size_t> searchSig(std::span<const Byte> v, std::span<const Byte> sig, std::size_t limit = 0) {
	auto search_span = (limit == 0 || limit > v.size())
		? v
		: v.first(limit);

	auto it = std::ranges::search(search_span, sig);
	if (it.empty()) {
		return std::nullopt;
	}

	return static_cast<std::size_t>(it.begin() - v.begin());
}
}  // namespace

prepared_cover prepare_cover_image(const std::filesystem::path& image_file_path, bool is_bluesky_mode) {
	constexpr std::size_t
		MINIMUM_IMAGE_SIZE          = 134,
		MAX_INPUT_JPG_BYTES         = 8 * 1024 * 1024,
		MAX_IMAGE_SIZE_AFTER_ENCODE = 4 * 1024 * 1024,
		MAX_IMAGE_SIZE_BLUESKY      = 912 * 1024;

	if (!text::hasValidFilename(image_file_path)) {
		throw std::runtime_error("Invalid Input Error: Unsupported characters in filename arguments.");
	}
	if (!text::hasFileExtension(image_file_path, {".jpg", ".jpeg", ".jfif"})) {
		throw std::runtime_error("File Type Error: Invalid image extension. Only expecting \".jpg\", \".jpeg\", or \".jfif\".");
	}

	io::input_file in{io::open_input_readonly_secure(image_file_path)};
	if (in.fd < 0) {
		if (errno == ENOENT) {
			throw std::runtime_error("Image File Error: File not found.");
		}
		if (errno == ELOOP) {
			throw std::runtime_error("Image File Error: Symbolic links are not allowed.");
		}
		if (errno == EXDEV) {
			throw std::runtime_error("Image File Error: Path traversal is not allowed.");
		}
		throw std::runtime_error(std::format("Read File Error: Unable to open image file ({}).", std::strerror(errno)));
	}

	struct stat st{};
	if (::fstat(in.fd, &st) != 0) {
		throw std::runtime_error(std::format("Read File Error: Unable to stat image file ({}).", std::strerror(errno)));
	}
	if (!S_ISREG(st.st_mode)) {
		throw std::runtime_error("Image File Error: Path is not a regular file.");
	}
	if (st.st_size < 0) {
		throw std::runtime_error("Image File Error: Invalid file size.");
	}

	const std::uintmax_t file_size_u = static_cast<std::uintmax_t>(st.st_size);
	if (file_size_u > std::numeric_limits<std::size_t>::max()) {
		throw std::runtime_error("Image File Error: File size exceeds supported limits.");
	}
	std::size_t jpg_vec_size = static_cast<std::size_t>(file_size_u);

	if (MINIMUM_IMAGE_SIZE > jpg_vec_size) {
		throw std::runtime_error("Image File Error: Invalid file size.");
	}
	if (jpg_vec_size > MAX_INPUT_JPG_BYTES) {
		throw std::runtime_error("Image File Error: Cover image file exceeds maximum size limit.");
	}

	vBytes jpg_vec(jpg_vec_size);
	io::read_all_fd(in.fd, std::span<Byte>(jpg_vec.data(), jpg_vec.size()));
	in.reset();

	jpeg::optimizeImage(jpg_vec);

	constexpr std::size_t DQT_SEARCH_LIMIT = 100;
	constexpr auto DQT1_SIG = std::to_array<Byte>({0xFF, 0xDB, 0x00, 0x43});
	constexpr auto DQT2_SIG = std::to_array<Byte>({0xFF, 0xDB, 0x00, 0x84});

	auto dqt1 = searchSig(jpg_vec, DQT1_SIG, DQT_SEARCH_LIMIT);
	auto dqt2 = searchSig(jpg_vec, DQT2_SIG, DQT_SEARCH_LIMIT);
	if (!dqt1 && !dqt2) {
		throw std::runtime_error("Image File Error: No DQT segment found (corrupt or unsupported JPG).");
	}

	constexpr std::size_t NPOS = static_cast<std::size_t>(-1);
	const std::size_t dqt_pos = std::min(dqt1.value_or(NPOS), dqt2.value_or(NPOS));
	jpg_vec.erase(jpg_vec.begin(), jpg_vec.begin() + static_cast<std::ptrdiff_t>(dqt_pos));

	jpg_vec_size = jpg_vec.size();
	if (jpg_vec_size > MAX_IMAGE_SIZE_AFTER_ENCODE) {
		throw std::runtime_error("Image File Error: Image exceeds maximum size limit.");
	}
	if (is_bluesky_mode && jpg_vec_size > MAX_IMAGE_SIZE_BLUESKY) {
		throw std::runtime_error("Image File Error: Image exceeds maximum size limit for Bluesky.");
	}

	return prepared_cover{
		.jpg_vec = std::move(jpg_vec),
	};
}

output_result write_output_image(std::span<const Byte> segment_vec, std::span<const Byte> jpg_vec) {
	if (segment_vec.size() > std::numeric_limits<std::size_t>::max() - jpg_vec.size()) {
		throw std::runtime_error("File Size Error: Output image size overflow.");
	}
	const std::size_t output_size = segment_vec.size() + jpg_vec.size();

	io::output_file out = io::create_output_file();
	io::write_all_fd(out.fd, segment_vec);
	io::write_all_fd(out.fd, jpg_vec);
	if (::fsync(out.fd) != 0) {
		throw std::runtime_error(std::format("Write Error: {}", std::strerror(errno)));
	}
	if (::close(out.fd) != 0) {
		throw std::runtime_error(std::format("Write Error: {}", std::strerror(errno)));
	}
	out.fd = -1;

	return output_result{
		.path = std::move(out.path),
		.total_bytes = output_size,
	};
}
}  // namespace imgprmt::pipeline
