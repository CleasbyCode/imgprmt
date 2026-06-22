#pragma once

// ---------------------------------------------------------------------------
// Toolchain floor. imgprmt is distributed as source and built by each user, so
// fail early with a clear message rather than a wall of template errors when
// the compiler is too old. The floor is set by std::print / std::format
// (and other C++23 library features used throughout).
// ---------------------------------------------------------------------------
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 14
#  error "imgprmt requires GCC >= 14 (for std::print/std::format). Please upgrade your compiler."
#elif defined(__clang__) && __clang_major__ < 18
#  error "imgprmt requires Clang >= 18 with a C++23 standard library (libc++ 18+ or libstdc++ 14+). Please upgrade your compiler."
#endif

#include <string_view>

namespace imgprmt {
inline constexpr std::string_view VERSION = "1.4";

int run_cli(int argc, char** argv);
}  // namespace imgprmt
