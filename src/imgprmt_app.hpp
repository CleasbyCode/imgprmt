#pragma once

#include <cstddef>
#include <cstdint>

namespace imgprmt {
int run_cli(int argc, char** argv);
int fuzz_url_input(const std::uint8_t* data, std::size_t size);
}  // namespace imgprmt
