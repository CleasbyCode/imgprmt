#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace imgprmt::jpeg {
using Byte = std::uint8_t;
using vBytes = std::vector<Byte>;

[[nodiscard]] std::optional<std::uint16_t> exifOrientation(std::span<const Byte> jpg);
[[nodiscard]] int estimateImageQuality(std::span<const Byte> jpg);
void optimizeImage(vBytes& jpg_vec);
}  // namespace imgprmt::jpeg
