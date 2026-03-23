#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>

namespace imgprmt {
enum class Option : std::uint8_t { None, Bluesky };

struct program_args {
	Option option{Option::None};
	std::filesystem::path image_file_path;

	[[nodiscard]] static std::optional<program_args> parse(int argc, char** argv);
};
}  // namespace imgprmt
