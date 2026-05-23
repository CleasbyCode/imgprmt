#pragma once

#include <string_view>

namespace imgprmt {
inline constexpr std::string_view VERSION = "1.3";

int run_cli(int argc, char** argv);
}  // namespace imgprmt
