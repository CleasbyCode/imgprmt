#include "imgprmt_url.hpp"

#include <cstddef>
#include <expected>
#include <format>
#include <stdexcept>
#include <string_view>

namespace {
bool is_dec(char c) noexcept {
    return c >= '0' && c <= '9';
}

bool is_hex(char c) noexcept {
    return (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'F') ||
           (c >= 'a' && c <= 'f');
}

bool is_ascii_alpha(char c) noexcept {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool is_ascii_alnum(char c) noexcept {
    return is_ascii_alpha(c) || is_dec(c);
}

bool valid_pct(std::string_view s, std::size_t i) noexcept {
    return i + 2 < s.size() && is_hex(s[i + 1]) && is_hex(s[i + 2]);
}

bool is_path_query_char(char c) noexcept {
    switch (c) {
        case '-':
        case '_':
        case '.':
        case '~':
        case ':':
        case '/':
        case '?':
        case '@':
        case '!':
        case '$':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case ';':
        case '=':
            return true;
        default:
            return is_ascii_alnum(c);
    }
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

bool valid_hostname_ascii(std::string_view host) noexcept {
    if (host.empty() || host.size() > 253) {
        return false;
    }

    bool all_digits_or_dot = true;
    std::size_t label_len = 0;
    char label_last = '\0';

    for (char c : host) {
        const auto uc = static_cast<unsigned char>(c);
        if (uc >= 0x80) {
            return false;
        }

        if (c == '.') {
            if (label_len == 0 || label_len > 63 || !is_ascii_alnum(label_last)) {
                return false;
            }
            label_len = 0;
            label_last = '\0';
            continue;
        }

        if (!(is_ascii_alnum(c) || c == '-')) {
            return false;
        }
        if (label_len == 0 && !is_ascii_alnum(c)) {
            return false;
        }
        ++label_len;
        if (label_len > 63) {
            return false;
        }
        label_last = c;

        if (!is_dec(c)) {
            all_digits_or_dot = false;
        }
    }

    if (label_len == 0 || !is_ascii_alnum(label_last)) {
        return false;
    }

    if (all_digits_or_dot) {
        return valid_ipv4(host);
    }
    return true;
}

std::expected<void, std::string_view> validate_url_ascii_chars(std::string_view url) {
    for (char c : url) {
        const auto uc = static_cast<unsigned char>(c);
        if (uc <= 0x20 || uc == 0x7F) {
            return std::unexpected("Control/whitespace not allowed");
        }
        if (uc >= 0x80) {
            return std::unexpected("URL must contain ASCII characters only");
        }
    }
    return {};
}

std::expected<void, std::string_view> validate_port_ascii(std::string_view port_str) {
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

    return {};
}

std::expected<std::string_view, std::string_view> parse_authority(std::string_view authority) {
    if (authority.empty()) {
        return std::unexpected("Missing host");
    }

    if (authority.find('@') != std::string::npos) {
        return std::unexpected("Credentials not allowed");
    }

    const std::size_t colon = authority.rfind(':');
    if (colon == std::string::npos) {
        return authority;
    }

    std::string_view port_str = authority.substr(colon + 1);
    auto port_result = validate_port_ascii(port_str);
    if (!port_result) {
        return std::unexpected(port_result.error());
    }

    return authority.substr(0, colon);
}

std::expected<void, std::string_view> valid_path_query_ascii(std::string_view s) {
    for (std::size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x20 || c == 0x7F) {
            return std::unexpected("Control char in path/query");
        }
        if (c >= 0x80) {
            return std::unexpected("Non-ASCII character in path/query");
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
        if (!is_path_query_char(static_cast<char>(c))) {
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

    auto ascii_result = validate_url_ascii_chars(url);
    if (!ascii_result) {
        return std::unexpected(ascii_result.error());
    }

    const std::size_t pos = kScheme.size();
    const std::size_t slash = url.find('/', pos);
    const std::string_view url_view{url};
    const std::string_view authority = url_view.substr(pos, (slash == std::string::npos) ? std::string::npos : slash - pos);

    auto authority_result = parse_authority(authority);
    if (!authority_result) {
        return std::unexpected(authority_result.error());
    }

    if (!valid_hostname_ascii(*authority_result)) {
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
}  // namespace

namespace imgprmt::url {
void validate_url_link_core(const std::string& url) {
    auto result = validate_url_link(url);
    if (!result) {
        throw std::runtime_error(std::format("Link Error: {}", result.error()));
    }
}
}  // namespace imgprmt::url
