#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>

namespace imgprmt::io {
struct output_file {
	int fd{-1};
	std::string path;

	output_file();
	output_file(int fd_, std::string path_);

	output_file(const output_file&) = delete;
	output_file& operator=(const output_file&) = delete;

	output_file(output_file&& other) noexcept;
	output_file& operator=(output_file&& other) noexcept;

	~output_file();

	void reset() noexcept;
};

struct input_file {
	int fd{-1};

	input_file();
	explicit input_file(int fd_);

	input_file(const input_file&) = delete;
	input_file& operator=(const input_file&) = delete;

	input_file(input_file&& other) noexcept;
	input_file& operator=(input_file&& other) noexcept;

	~input_file();

	void reset() noexcept;
};

[[nodiscard]] output_file create_output_file();
void write_all_fd(int fd, std::span<const std::uint8_t> data);
void read_all_fd(int fd, std::span<std::uint8_t> data);
[[nodiscard]] int open_input_readonly_secure(const std::filesystem::path& path);
void apply_runtime_limits();
}  // namespace imgprmt::io
