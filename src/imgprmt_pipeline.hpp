#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

namespace imgprmt::pipeline {
using Byte = std::uint8_t;
using vBytes = std::vector<Byte>;

struct prepared_cover {
	vBytes jpg_vec;
	// Effective JPEG starts at jpg_vec.data() + start_offset (skipping pre-DQT header bytes).
	// Avoids a multi-MB memmove that would otherwise shift the whole post-DQT region to position 0.
	std::size_t start_offset{0};
};

struct output_result {
	std::string path;
	std::size_t total_bytes{0};
};

[[nodiscard]] prepared_cover prepare_cover_image(const std::filesystem::path& image_file_path, bool is_bluesky_mode);
[[nodiscard]] output_result write_output_image(std::span<const Byte> segment_vec, std::span<const Byte> jpg_vec);
}  // namespace imgprmt::pipeline
