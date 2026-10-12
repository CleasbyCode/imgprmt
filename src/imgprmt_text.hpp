#pragma once

#include <cstddef>
#include <filesystem>
#include <initializer_list>
#include <string>
#include <string_view>

namespace imgprmt::text {
[[nodiscard]] bool has_valid_filename(const std::filesystem::path& p);
[[nodiscard]] bool has_file_extension(const std::filesystem::path& p, std::initializer_list<std::string_view> exts);
void replace_problem_chars(std::wstring& s);
[[nodiscard]] std::size_t calculate_default_prompt_budget(std::size_t url_bytes);
[[nodiscard]] std::size_t calculate_twitter_prompt_budget(std::size_t url_bytes) noexcept;
void enforce_prompt_size_limit(std::string_view utf8_prompt, std::size_t max_bytes);
bool force_utf8_locale();
std::string convert_string(const std::wstring& wide);
std::wstring read_long_line_from_tty(std::size_t max_chars, std::size_t max_utf8_bytes, std::string_view field_name);
}  // namespace imgprmt::text
