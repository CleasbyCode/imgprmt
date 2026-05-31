#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace imgprmt::polyglot {
using Byte = std::uint8_t;
using vBytes = std::vector<Byte>;

enum class SegmentMode : Byte { Default, Bluesky };

[[nodiscard]] vBytes build_segment(SegmentMode mode, std::string utf8_prompt, std::string utf8_url);
}  // namespace imgprmt::polyglot
