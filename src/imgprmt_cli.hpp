#pragma once

#include <string>

namespace imgprmt::cli {
struct prompt_data {
	std::string utf8_url;
	std::string utf8_prompt;
};

[[nodiscard]] prompt_data read_prompt_data(bool is_bluesky_mode);
}  // namespace imgprmt::cli
