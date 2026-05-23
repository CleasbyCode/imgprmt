#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace imgprmt::jpeg {
using Byte = std::uint8_t;
using vBytes = std::vector<Byte>;

[[nodiscard]] std::optional<std::uint16_t> exif_orientation(std::span<const Byte> jpg);
[[nodiscard]] int estimate_image_quality(std::span<const Byte> jpg);
void optimize_image(vBytes& jpg_vec);

// Returns the offset of the 0xFF prefix of the first segment with the given
// marker byte (e.g. 0xDB for DQT), starting after the SOI marker. Optionally
// limit the scan to the first `limit` bytes. Returns std::nullopt if not found.
[[nodiscard]] std::optional<std::size_t> find_marker_offset(std::span<const Byte> jpg, Byte marker, std::size_t limit = 0);
}  // namespace imgprmt::jpeg
