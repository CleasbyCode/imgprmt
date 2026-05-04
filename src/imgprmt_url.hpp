#pragma once

#include <expected>
#include <string>
#include <string_view>

namespace imgprmt::url {
[[nodiscard]] std::expected<void, std::string_view> valid_path_query_ascii(std::string_view s);
[[nodiscard]] std::expected<void, std::string_view> validate_url_link(const std::string& url);
void validate_url_link_core(const std::string& url);
}  // namespace imgprmt::url
