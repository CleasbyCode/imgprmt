#include "imgprmt_pipeline.hpp"

#include "imgprmt_io.hpp"
#include "imgprmt_jpeg.hpp"
#include "imgprmt_text.hpp"

#include <cerrno>
#include <cstring>
#include <format>
#include <limits>
#include <stdexcept>
#include <utility>
#include <sys/stat.h>
#include <unistd.h>

namespace imgprmt::pipeline {
namespace {
constexpr std::size_t MINIMUM_IMAGE_SIZE = 134;
constexpr std::size_t MAX_INPUT_JPG_BYTES = 8 * 1024 * 1024;
constexpr std::size_t MAX_IMAGE_SIZE_AFTER_ENCODE = 4 * 1024 * 1024;
constexpr std::size_t MAX_IMAGE_SIZE_BLUESKY = 912 * 1024;
constexpr Byte DQT_MARKER = 0xDB;

void cleanup_partial_output(io::output_file& out) noexcept {
	out.reset();
	if (!out.path.empty()) {
		::unlink(out.path.c_str());
	}
}

void close_output_file(io::output_file& out) {
	const int fd = out.fd;
	out.fd = -1;
	if (::close(fd) != 0) {
		throw std::runtime_error(std::format("Write Error: {}", std::strerror(errno)));
	}
}

void validate_cover_path(const std::filesystem::path& image_file_path) {
	if (!text::has_valid_filename(image_file_path)) {
		throw std::runtime_error("Invalid Input Error: Unsupported characters in filename arguments.");
	}
	if (!text::has_file_extension(image_file_path, {".jpg", ".jpeg", ".jfif"})) {
		throw std::runtime_error("File Type Error: Invalid image extension. Only expecting \".jpg\", \".jpeg\", or \".jfif\".");
	}
}

[[noreturn]] void throw_open_input_error() {
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

[[nodiscard]] io::input_file open_cover_file(const std::filesystem::path& image_file_path) {
	io::input_file in{io::open_input_readonly_secure(image_file_path)};
	if (in.fd < 0) {
		throw_open_input_error();
	}
	return in;
}

[[nodiscard]] std::size_t regular_file_size(int fd) {
	struct stat st{};
	if (::fstat(fd, &st) != 0) {
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
	return static_cast<std::size_t>(file_size_u);
}

void validate_input_file_size(std::size_t jpg_vec_size) {
	if (MINIMUM_IMAGE_SIZE > jpg_vec_size) {
		throw std::runtime_error("Image File Error: Invalid file size.");
	}
	if (jpg_vec_size > MAX_INPUT_JPG_BYTES) {
		throw std::runtime_error("Image File Error: Cover image file exceeds maximum size limit.");
	}
}

[[nodiscard]] vBytes read_cover_bytes(io::input_file& in, std::size_t jpg_vec_size) {
	vBytes jpg_vec(jpg_vec_size);
	io::read_all_fd(in.fd, std::span<Byte>(jpg_vec.data(), jpg_vec.size()));
	in.reset();
	return jpg_vec;
}

[[nodiscard]] std::size_t dqt_start_offset(std::span<const Byte> jpg_vec) {
	auto dqt_pos_opt = jpeg::find_marker_offset(jpg_vec, DQT_MARKER);
	if (!dqt_pos_opt) {
		throw std::runtime_error("Image File Error: No DQT segment found (corrupt or unsupported JPG).");
	}
	return *dqt_pos_opt;
}

void validate_effective_image_size(std::size_t effective_size, bool is_bluesky_mode) {
	if (effective_size > MAX_IMAGE_SIZE_AFTER_ENCODE) {
		throw std::runtime_error("Image File Error: Image exceeds maximum size limit.");
	}
	if (is_bluesky_mode && effective_size > MAX_IMAGE_SIZE_BLUESKY) {
		throw std::runtime_error("Image File Error: Image exceeds maximum size limit for Bluesky.");
	}
}
}  // namespace

prepared_cover prepare_cover_image(const std::filesystem::path& image_file_path, bool is_bluesky_mode) {
	validate_cover_path(image_file_path);

	io::input_file in = open_cover_file(image_file_path);
	const std::size_t jpg_vec_size = regular_file_size(in.fd);
	validate_input_file_size(jpg_vec_size);

	vBytes jpg_vec = read_cover_bytes(in, jpg_vec_size);

	jpeg::optimize_image(jpg_vec);

	const std::size_t dqt_pos = dqt_start_offset(jpg_vec);
	const std::size_t effective_size = jpg_vec.size() - dqt_pos;
	validate_effective_image_size(effective_size, is_bluesky_mode);

	return prepared_cover{
		.jpg_vec = std::move(jpg_vec),
		.start_offset = dqt_pos,
	};
}

output_result write_output_image(std::span<const Byte> segment_vec, std::span<const Byte> jpg_vec) {
	if (segment_vec.size() > std::numeric_limits<std::size_t>::max() - jpg_vec.size()) {
		throw std::runtime_error("File Size Error: Output image size overflow.");
	}
	const std::size_t output_size = segment_vec.size() + jpg_vec.size();

	io::output_file out = io::create_output_file();
	try {
		io::write_all_fd(out.fd, segment_vec);
		io::write_all_fd(out.fd, jpg_vec);
		if (::fsync(out.fd) != 0) {
			throw std::runtime_error(std::format("Write Error: {}", std::strerror(errno)));
		}

		close_output_file(out);
	} catch (...) {
		cleanup_partial_output(out);
		throw;
	}

	return output_result{
		.path = std::move(out.path),
		.total_bytes = output_size,
	};
}
}  // namespace imgprmt::pipeline
