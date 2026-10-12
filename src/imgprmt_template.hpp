#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

namespace imgprmt::polyglot {
using Byte = std::uint8_t;
using vBytes = std::vector<Byte>;

enum class SegmentMode : Byte { Default, Bluesky };

// Pixel dimensions of the cover image as written to the output file.
struct ImageDimensions {
	std::uint32_t width{0};
	std::uint32_t height{0};
};

[[nodiscard]] vBytes build_segment(SegmentMode mode, std::string_view utf8_prompt, std::string_view utf8_url, ImageDimensions dimensions);
}  // namespace imgprmt::polyglot
