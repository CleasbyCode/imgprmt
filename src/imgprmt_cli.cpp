#include "imgprmt_cli.hpp"

#include "imgprmt_app.hpp"
#include "imgprmt_text.hpp"
#include "imgprmt_url.hpp"

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <locale>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace imgprmt::cli {
namespace {
constexpr std::size_t URL_MAX_CHARS = 200;

void configure_utf8_stdio() {
	if (!text::force_utf8_locale()) {
		throw std::runtime_error(
			"UTF-8 locale is required. Please install/enable a UTF-8 locale "
			"(e.g., C.UTF-8 or en_US.UTF-8) and try again.");
	}

	const std::locale loc("");
	std::wcin.imbue(loc);
	std::wcout.imbue(loc);
	std::wcerr.imbue(loc);
}

[[nodiscard]] std::string read_validated_url() {
	std::print("\nEnter a complete web address\n(e.g., image source, your website or social media profile)\n\nURL: ");
	std::fflush(stdout);

	std::wstring wurl = text::read_long_line_from_tty(URL_MAX_CHARS, URL_MAX_CHARS, "Link");
	std::string utf8_url = text::convert_string(wurl);
	url::validate_url_link_core(utf8_url);
	return utf8_url;
}

void print_prompt_limits(std::size_t url_bytes, std::size_t default_max_bytes, bool is_bluesky_mode) {
	if (is_bluesky_mode) {
		std::println("\nDefault byte limit: {}", default_max_bytes);
	} else {
		std::println("\nDefault byte limit: {} | X-Twitter byte limit: {}",
			default_max_bytes,
			text::calculate_twitter_prompt_budget(url_bytes));
	}
}

[[nodiscard]] std::string read_sanitized_prompt(std::size_t max_bytes) {
	std::print("\nType or paste in your image description as one long sentence");
	std::print("\nIf required, add <br> tags to your text for new lines\n\nPrompt: ");
	std::fflush(stdout);

	std::wstring wprompt = text::read_long_line_from_tty(max_bytes, max_bytes, "Prompt");
	text::replace_problem_chars(wprompt);

	std::string utf8_prompt = text::convert_string(wprompt);
	text::enforce_prompt_size_limit(utf8_prompt, max_bytes);
	return utf8_prompt;
}
}  // namespace

prompt_data read_prompt_data(bool is_bluesky_mode) {
	std::println("\n*** imgprmt v{} ***", VERSION);

	configure_utf8_stdio();

	std::string utf8_url = read_validated_url();
	const std::size_t default_max_bytes = text::calculate_default_prompt_budget(utf8_url.size());
	print_prompt_limits(utf8_url.size(), default_max_bytes, is_bluesky_mode);

	std::string utf8_prompt = read_sanitized_prompt(default_max_bytes);

	return prompt_data{
		.utf8_url = std::move(utf8_url),
		.utf8_prompt = std::move(utf8_prompt),
	};
}
}  // namespace imgprmt::cli
