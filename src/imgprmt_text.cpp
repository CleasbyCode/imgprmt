#include "imgprmt_text.hpp"

#include <algorithm>
#include <cstdint>
#include <clocale>
#include <cctype>
#include <cwchar>
#include <format>
#include <iostream>
#include <limits>
#include <locale>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include <termios.h>
#include <unistd.h>

namespace {
[[nodiscard]] std::size_t utf8_encoded_bytes(wchar_t ch, std::string_view field_name) {
    using UnsignedWchar = std::make_unsigned_t<wchar_t>;
    const auto codepoint = static_cast<std::uint32_t>(static_cast<UnsignedWchar>(ch));

    if (codepoint > 0x10FFFFu || (codepoint >= 0xD800u && codepoint <= 0xDFFFu)) {
        throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
    }

    if (codepoint <= 0x7Fu) {
        return 1;
    }
    if (codepoint <= 0x7FFu) {
        return 2;
    }
    if (codepoint <= 0xFFFFu) {
        return 3;
    }
    return 4;
}

[[nodiscard]] bool is_utf8_continuation_byte(unsigned char c) noexcept {
    return (c & 0xC0u) == 0x80u;
}

[[nodiscard]] std::wstring decode_utf8_or_throw(std::string_view input, std::string_view field_name) {
    std::wstring out;
    out.reserve(input.size());

    for (std::size_t i = 0; i < input.size();) {
        const auto b0 = static_cast<unsigned char>(input[i]);
        std::uint32_t codepoint = 0;
        std::size_t width = 0;

        if (b0 <= 0x7Fu) {
            codepoint = b0;
            width = 1;
        } else if (b0 >= 0xC2u && b0 <= 0xDFu) {
            width = 2;
            if (i + width > input.size()) {
                throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
            }
            const auto b1 = static_cast<unsigned char>(input[i + 1]);
            if (!is_utf8_continuation_byte(b1)) {
                throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
            }
            codepoint = ((static_cast<std::uint32_t>(b0 & 0x1Fu)) << 6) |
                        static_cast<std::uint32_t>(b1 & 0x3Fu);
        } else if (b0 >= 0xE0u && b0 <= 0xEFu) {
            width = 3;
            if (i + width > input.size()) {
                throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
            }
            const auto b1 = static_cast<unsigned char>(input[i + 1]);
            const auto b2 = static_cast<unsigned char>(input[i + 2]);
            if (!is_utf8_continuation_byte(b1) || !is_utf8_continuation_byte(b2)) {
                throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
            }
            if ((b0 == 0xE0u && b1 < 0xA0u) || (b0 == 0xEDu && b1 >= 0xA0u)) {
                throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
            }
            codepoint = ((static_cast<std::uint32_t>(b0 & 0x0Fu)) << 12) |
                        ((static_cast<std::uint32_t>(b1 & 0x3Fu)) << 6) |
                        static_cast<std::uint32_t>(b2 & 0x3Fu);
        } else if (b0 >= 0xF0u && b0 <= 0xF4u) {
            width = 4;
            if (i + width > input.size()) {
                throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
            }
            const auto b1 = static_cast<unsigned char>(input[i + 1]);
            const auto b2 = static_cast<unsigned char>(input[i + 2]);
            const auto b3 = static_cast<unsigned char>(input[i + 3]);
            if (!is_utf8_continuation_byte(b1) || !is_utf8_continuation_byte(b2) || !is_utf8_continuation_byte(b3)) {
                throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
            }
            if ((b0 == 0xF0u && b1 < 0x90u) || (b0 == 0xF4u && b1 > 0x8Fu)) {
                throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
            }
            codepoint = ((static_cast<std::uint32_t>(b0 & 0x07u)) << 18) |
                        ((static_cast<std::uint32_t>(b1 & 0x3Fu)) << 12) |
                        ((static_cast<std::uint32_t>(b2 & 0x3Fu)) << 6) |
                        static_cast<std::uint32_t>(b3 & 0x3Fu);
        } else {
            throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
        }

        if (codepoint > 0x10FFFFu || (codepoint >= 0xD800u && codepoint <= 0xDFFFu)) {
            throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
        }
        if (codepoint > static_cast<std::uint32_t>(std::numeric_limits<wchar_t>::max())) {
            throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
        }

        out.push_back(static_cast<wchar_t>(codepoint));
        i += width;
    }

    return out;
}

void enforce_utf8_byte_limit_impl(std::wstring_view input, std::size_t max_bytes, std::string_view field_name) {
    std::size_t used_bytes = 0;
    for (wchar_t ch : input) {
        const std::size_t char_bytes = utf8_encoded_bytes(ch, field_name);
        if (char_bytes > max_bytes - used_bytes) {
            throw std::runtime_error(std::format(
                "{} Error: Input exceeds maximum allowed size ({} bytes).",
                field_name,
                max_bytes));
        }
        used_bytes += char_bytes;
    }
}

struct TermiosGuard {
    termios old{};
    bool active = false;

    TermiosGuard() {
        if (::isatty(STDIN_FILENO)) {
            if (::tcgetattr(STDIN_FILENO, &old) == 0) {
                termios raw = old;
                raw.c_lflag &= ~static_cast<tcflag_t>(ICANON);
                raw.c_cc[VMIN] = 1;
                raw.c_cc[VTIME] = 0;
                if (::tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == 0) {
                    active = true;
                }
            }
        }
    }

    ~TermiosGuard() {
        if (active) {
            ::tcsetattr(STDIN_FILENO, TCSAFLUSH, &old);
        }
    }
};
}  // namespace

namespace imgprmt::text {
bool hasValidFilename(const std::filesystem::path& p) {
    if (p.empty()) {
        return false;
    }

    std::string filename = p.filename().string();
    if (filename.empty()) {
        return false;
    }

    auto validChar = [](unsigned char c) {
        return std::isalnum(c) || c == '.' || c == '-' || c == '_' || c == '@' || c == '%';
    };
    return std::ranges::all_of(filename, validChar);
}

bool hasFileExtension(const std::filesystem::path& p, std::initializer_list<std::string_view> exts) {
    auto e = p.extension().string();
    std::ranges::transform(e, e.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    return std::ranges::any_of(exts, [&e](std::string_view ext) {
        if (ext.size() != e.size()) {
            return false;
        }
        return std::ranges::equal(ext, e, [](char a, char b) {
            return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
        });
    });
}

void replace_problem_chars(std::wstring& s) {
    auto starts_with = [](const std::wstring& t, size_t pos, const std::wstring& needle) -> bool {
        return pos + needle.size() <= t.size() && t.compare(pos, needle.size(), needle) == 0;
    };

    std::wstring out;
    out.reserve(s.size() + 16);

    for (size_t i = 0; i < s.size(); ++i) {
        wchar_t c = s[i];

        if (c == L'<') {
            if (starts_with(s, i, L"<br>")) {
                out += L"<br>";
                i += 3;
                continue;
            }
            if (starts_with(s, i, L"<br/>")) {
                out += L"<br/>";
                i += 4;
                continue;
            }
            if (starts_with(s, i, L"<br />")) {
                out += L"<br />";
                i += 5;
                continue;
            }
        }

        if (c == L'\r') {
            if (i + 1 < s.size() && s[i + 1] == L'\n') {
                ++i;
            }
            out += L"<br>";
            continue;
        }
        if (c == L'\n') {
            out += L"<br>";
            continue;
        }
        if (c == L'\t') {
            out += L"    ";
            continue;
        }

        if ((c >= 0 && c < 0x20) || c == 0x7F || (c >= 0x80 && c <= 0x9F)) {
            continue;
        }
        if ((c >= 0x202A && c <= 0x202E) || c == 0x2066 || c == 0x2067 || c == 0x2068 || c == 0x2069) {
            continue;
        }

        switch (c) {
            case L'&':
                out += L"&amp;";
                break;
            case L'<':
                out += L"&lt;";
                break;
            case L'>':
                out += L"&gt;";
                break;
            case L'"':
                out += L"&quot;";
                break;
            case L'\'':
                out += L"&#39;";
                break;
            case L'`':
                out += L"&#96;";
                break;
            default:
                out.push_back(c);
                break;
        }
    }

    s.swap(out);
}

void enforce_utf8_byte_limit(std::wstring_view input, std::size_t max_bytes, std::string_view field_name) {
    if (max_bytes == 0) {
        throw std::runtime_error(std::format("{} Error: Maximum byte length is zero for this input.", field_name));
    }
    enforce_utf8_byte_limit_impl(input, max_bytes, field_name);
}

std::size_t calculate_default_prompt_budget(std::size_t url_bytes) {
    if (url_bytes >= imgprmt::text::DEFAULT_PROMPT_BUDGET) {
        throw std::runtime_error("Link Error: URL length leaves no room for prompt content.");
    }
    return imgprmt::text::DEFAULT_PROMPT_BUDGET - url_bytes;
}

std::size_t calculate_twitter_prompt_budget(std::size_t url_bytes) noexcept {
    return (url_bytes < imgprmt::text::TWITTER_PROMPT_BUDGET)
               ? (imgprmt::text::TWITTER_PROMPT_BUDGET - url_bytes)
               : 0;
}

void enforce_prompt_size_limit(std::string_view utf8_prompt, std::size_t max_bytes) {
    if (utf8_prompt.size() > max_bytes) {
        throw std::runtime_error(std::format(
            "Prompt Error: Prompt exceeds maximum allowed size ({} bytes) for this URL.",
            max_bytes));
    }
}

bool force_utf8_locale() {
    if (std::setlocale(LC_ALL, "C.UTF-8") || std::setlocale(LC_ALL, "en_US.UTF-8") || std::setlocale(LC_ALL, "UTF-8")) {
        try {
            std::locale::global(std::locale(""));
        } catch (...) {
        }
        return true;
    }
    return false;
}

std::string convert_string(const std::wstring& wide) {
    if (wide.empty()) {
        return {};
    }

    std::mbstate_t state{};
    const wchar_t* src = wide.c_str();
    std::size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
    if (len == static_cast<size_t>(-1)) {
        throw std::runtime_error("Conversion to UTF-8 failed.");
    }

    std::string converted_string(len, 0);
    src = wide.c_str();
    std::size_t written = std::wcsrtombs(converted_string.data(), &src, len, &state);
    if (written == static_cast<size_t>(-1)) {
        throw std::runtime_error("Conversion to UTF-8 failed.");
    }
    return converted_string;
}

std::wstring read_long_line_from_tty(std::size_t max_chars, std::size_t max_utf8_bytes, std::string_view field_name) {
    if (max_chars == 0) {
        throw std::runtime_error(std::format("{} Error: Maximum length is zero for this input.", field_name));
    }
    if (max_utf8_bytes == 0) {
        throw std::runtime_error(std::format("{} Error: Maximum byte length is zero for this input.", field_name));
    }

    std::wstring s;
    s.reserve(std::min<std::size_t>(max_chars, 4096));

    std::size_t used_bytes = 0;

    auto append_checked = [&](wchar_t ch) {
        if (s.size() >= max_chars) {
            throw std::runtime_error(std::format(
                "{} Error: Input exceeds the maximum length ({} characters).",
                field_name,
                max_chars));
        }

        const std::size_t char_bytes = utf8_encoded_bytes(ch, field_name);
        if (char_bytes > max_utf8_bytes - used_bytes) {
            throw std::runtime_error(std::format(
                "{} Error: Input exceeds maximum allowed size ({} bytes).",
                field_name,
                max_utf8_bytes));
        }

        used_bytes += char_bytes;
        s.push_back(ch);
    };

    if (!::isatty(STDIN_FILENO)) {
        std::string utf8_input;
        utf8_input.reserve(std::min<std::size_t>(max_utf8_bytes, 4096));

        char ch = '\0';
        while (std::cin.get(ch)) {
            if (ch == '\n') {
                break;
            }
            if (utf8_input.size() >= max_utf8_bytes) {
                throw std::runtime_error(std::format(
                    "{} Error: Input exceeds maximum allowed size ({} bytes).",
                    field_name,
                    max_utf8_bytes));
            }
            utf8_input.push_back(ch);
        }

        if (std::cin.bad()) {
            throw std::runtime_error(std::format("{} Error: Failed to read input.", field_name));
        }
        if (std::cin.fail() && !std::cin.eof()) {
            throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
        }

        std::wstring decoded = decode_utf8_or_throw(utf8_input, field_name);
        if (decoded.size() > max_chars) {
            throw std::runtime_error(std::format(
                "{} Error: Input exceeds the maximum length ({} characters).",
                field_name,
                max_chars));
        }
        return decoded;
    }

    TermiosGuard guard;
    wchar_t ch;
    while (std::wcin.get(ch)) {
        if (ch == L'\n') {
            break;
        }
        append_checked(ch);
    }

    if (std::wcin.bad()) {
        throw std::runtime_error(std::format("{} Error: Failed to read input.", field_name));
    }
    if (std::wcin.fail() && !std::wcin.eof()) {
        throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
    }
    enforce_utf8_byte_limit_impl(s, max_utf8_bytes, field_name);
    return s;
}
}  // namespace imgprmt::text
