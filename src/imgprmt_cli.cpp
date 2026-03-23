#include "imgprmt_cli.hpp"

#include "imgprmt_text.hpp"
#include "imgprmt_url.hpp"

#include <cstdio>
#include <iostream>
#include <locale>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>

namespace imgprmt::cli {
prompt_data read_prompt_data(bool is_bluesky_mode) {
	std::println("\n*** imgprmt v1.3 ***");

	if (!text::force_utf8_locale()) {
		throw std::runtime_error(
			"UTF-8 locale is required. Please install/enable a UTF-8 locale "
			"(e.g., C.UTF-8 or en_US.UTF-8) and try again.");
	}

	const std::locale loc("");
	std::wcin.imbue(loc);
	std::wcout.imbue(loc);
	std::wcerr.imbue(loc);

	std::wstring wprompt;
	std::wstring wurl;

	constexpr std::size_t URL_MIN_CHARS = 12;
	constexpr std::size_t URL_MAX_CHARS = 200;
	const std::wstring prefix = L"https://";

	std::print("\nEnter a complete web address\n(e.g., image source, your website or social media profile)\n\nURL: ");
	std::fflush(stdout);
	wurl = text::read_long_line_from_tty(URL_MAX_CHARS, URL_MAX_CHARS, "Link");

	if (URL_MIN_CHARS > wurl.length() || wurl.substr(0, prefix.length()) != prefix || wurl.length() > URL_MAX_CHARS) {
		throw std::runtime_error("Link Error: URL must start with 'https://', have a minimum length of 12 characters and not exceed 200 characters.");
	}

	std::string utf8_url = text::convert_string(wurl);
	url::validate_url_link_core(utf8_url);

	const std::size_t url_bytes = utf8_url.size();
	const std::size_t default_max_bytes = text::calculate_default_prompt_budget(url_bytes);
	const std::size_t twitter_max_bytes = text::calculate_twitter_prompt_budget(url_bytes);

	if (!is_bluesky_mode) {
		std::println("\nDefault byte limit: {} | X-Twitter byte limit: {}", default_max_bytes, twitter_max_bytes);
	} else {
		std::println("\nDefault byte limit: {}", default_max_bytes);
	}

	std::print("\nType or paste in your image description as one long sentence");
	std::print("\nIf required, add <br> tags to your text for new lines\n\nPrompt: ");
	std::fflush(stdout);

	wprompt = text::read_long_line_from_tty(default_max_bytes, default_max_bytes, "Prompt");
	text::replace_problem_chars(wprompt);

	std::string utf8_prompt = text::convert_string(wprompt);
	text::enforce_prompt_size_limit(utf8_prompt, default_max_bytes);

	return prompt_data{
		.utf8_url = std::move(utf8_url),
		.utf8_prompt = std::move(utf8_prompt),
	};
}
}  // namespace imgprmt::cli
