#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace imgprmt {
inline constexpr std::string_view VERSION = "1.3";

int run_cli(int argc, char** argv);
int fuzz_url_input(const std::uint8_t* data, std::size_t size);
}  // namespace imgprmt
