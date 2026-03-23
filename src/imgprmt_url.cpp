#include "imgprmt_url.hpp"

#include <cctype>
#include <format>
#include <stdexcept>

namespace {
bool is_dec(char c) noexcept {
    return c >= '0' && c <= '9';
}

bool is_hex(char c) noexcept {
    return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

bool valid_pct(std::string_view s, std::size_t i) noexcept {
    return i + 2 < s.size() && is_hex(s[i + 1]) && is_hex(s[i + 2]);
}

bool valid_ipv4(std::string_view s) noexcept {
    int dots = 0;
    int val = 0;
    int digits = 0;

    for (char c : s) {
        if (c == '.') {
            if (digits == 0 || val > 255) {
                return false;
            }
            ++dots;
            val = 0;
            digits = 0;
        } else if (is_dec(c)) {
            val = val * 10 + (c - '0');
            if (val > 255) {
                return false;
            }
            ++digits;
        } else {
            return false;
        }
    }

    if (digits == 0 || val > 255) {
        return false;
    }
    return dots == 3;
}

bool valid_dns_label(std::string_view lbl) noexcept {
    if (lbl.empty() || lbl.size() > 63) {
        return false;
    }

    auto alnum = [](unsigned char c) { return std::isalnum(c) != 0; };
    const auto first = static_cast<unsigned char>(lbl.front());
    const auto last = static_cast<unsigned char>(lbl.back());
    if (!alnum(first) || !alnum(last)) {
        return false;
    }

    for (char c : lbl) {
        const auto uc = static_cast<unsigned char>(c);
        if (!(alnum(uc) || uc == '-')) {
            return false;
        }
    }
    return true;
}

bool valid_hostname_ascii(std::string_view host) noexcept {
    if (host.empty() || host.size() > 253) {
        return false;
    }
    if (host.contains("..")) {
        return false;
    }
    if (host.back() == '.') {
        return false;
    }

    bool all_digits_or_dot = true;
    for (char c : host) {
        const auto uc = static_cast<unsigned char>(c);
        if (!(std::isalnum(uc) || uc == '-' || uc == '.')) {
            return false;
        }
        if (!(std::isdigit(uc) || uc == '.')) {
            all_digits_or_dot = false;
        }
    }

    if (all_digits_or_dot && valid_ipv4(host)) {
        return true;
    }

    std::size_t start = 0;
    while (true) {
        std::size_t dot = host.find('.', start);
        std::string_view lbl = host.substr(start, (dot == std::string::npos) ? std::string::npos : dot - start);
        if (!valid_dns_label(lbl)) {
            return false;
        }
        if (dot == std::string::npos) {
            break;
        }
        start = dot + 1;
    }
    return true;
}
}  // namespace

namespace imgprmt::url {
std::expected<void, std::string_view> valid_path_query_ascii(std::string_view s) {
    for (std::size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x20 || c == 0x7F) {
            return std::unexpected("Control char in path/query");
        }
        if (c == '#') {
            return std::unexpected("Fragment ('#') not allowed");
        }
        if (c == '%') {
            if (!valid_pct(s, i)) {
                return std::unexpected("Bad percent-encoding");
            }
            i += 2;
            continue;
        }
        if (!(std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == ':' || c == '/' || c == '?' ||
              c == '@' || c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' ||
              c == '+' || c == ',' || c == ';' || c == '=')) {
            return std::unexpected("Illegal character in path/query");
        }
    }
    return {};
}

std::expected<void, std::string_view> validate_url_link(const std::string& url) {
    constexpr std::string_view kScheme = "https://";
    if (!url.starts_with(kScheme)) {
        return std::unexpected("URL must start with 'https://'");
    }

    for (char c : url) {
        const auto uc = static_cast<unsigned char>(c);
        if (uc < 0x20 || uc == 0x7F) {
            return std::unexpected("Control/whitespace not allowed");
        }
    }

    const std::size_t pos = kScheme.size();
    const std::size_t slash = url.find('/', pos);
    const std::string_view url_view{url};
    const std::string_view authority = url_view.substr(pos, (slash == std::string::npos) ? std::string::npos : slash - pos);

    if (authority.empty()) {
        return std::unexpected("Missing host");
    }

    if (authority.find('@') != std::string::npos) {
        return std::unexpected("Credentials not allowed");
    }

    std::string_view host;
    std::string_view port_str;
    const std::size_t colon = authority.rfind(':');
    if (colon != std::string::npos) {
        host = authority.substr(0, colon);
        port_str = authority.substr(colon + 1);

        if (port_str.empty()) {
            return std::unexpected("Empty port");
        }

        unsigned long port = 0;
        for (char c : port_str) {
            if (!is_dec(c)) {
                return std::unexpected("Port must be digits");
            }
            port = port * 10UL + static_cast<unsigned long>(c - '0');
            if (port > 65535) {
                return std::unexpected("Port out of range");
            }
        }
        if (port == 0) {
            return std::unexpected("Port out of range");
        }
    } else {
        host = authority;
    }

    if (!valid_hostname_ascii(host)) {
        return std::unexpected("Invalid host");
    }

    if (slash == std::string::npos) {
        return {};
    }

    auto path_result = valid_path_query_ascii(url_view.substr(slash));
    if (!path_result) {
        return std::unexpected(path_result.error());
    }
    return {};
}

void validate_url_link_core(const std::string& url) {
    auto result = validate_url_link(url);
    if (!result) {
        throw std::runtime_error(std::format("Link Error: {}", result.error()));
    }
}
}  // namespace imgprmt::url
