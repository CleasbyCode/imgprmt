#include "imgprmt_text.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <clocale>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <format>
#include <iostream>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include <signal.h>
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

// Appends the UTF-8 encoding of a codepoint the caller has already validated.
void append_utf8(std::string& out, std::uint32_t cp) {
    if (cp <= 0x7Fu) {
        out.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FFu) {
        out.push_back(static_cast<char>(0xC0u | (cp >> 6)));
        out.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
    } else if (cp <= 0xFFFFu) {
        out.push_back(static_cast<char>(0xE0u | (cp >> 12)));
        out.push_back(static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu)));
        out.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
    } else {
        out.push_back(static_cast<char>(0xF0u | (cp >> 18)));
        out.push_back(static_cast<char>(0x80u | ((cp >> 12) & 0x3Fu)));
        out.push_back(static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu)));
        out.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
    }
}

[[nodiscard]] bool is_utf8_continuation_byte(unsigned char c) noexcept {
    return (c & 0xC0u) == 0x80u;
}

[[noreturn]] void throw_invalid_utf8(std::string_view field_name) {
    throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
}

[[nodiscard]] bool is_ascii_alnum(unsigned char c) noexcept {
    return (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z');
}

[[nodiscard]] char ascii_lower(unsigned char c) noexcept {
    if (c >= 'A' && c <= 'Z') {
        return static_cast<char>(c - 'A' + 'a');
    }
    return static_cast<char>(c);
}

[[nodiscard]] bool starts_with(std::wstring_view text, std::size_t pos, std::wstring_view needle) noexcept {
    return pos <= text.size() && needle.size() <= text.size() - pos &&
           text.substr(pos, needle.size()) == needle;
}

[[nodiscard]] std::size_t allowed_br_tag_size(std::wstring_view text, std::size_t pos) noexcept {
    if (starts_with(text, pos, L"<br>")) {
        return 4;
    }
    if (starts_with(text, pos, L"<br/>")) {
        return 5;
    }
    if (starts_with(text, pos, L"<br />")) {
        return 6;
    }
    return 0;
}

[[nodiscard]] bool is_control_codepoint(wchar_t c) noexcept {
    return (c >= 0 && c < 0x20) || c == 0x7F || (c >= 0x80 && c <= 0x9F);
}

[[nodiscard]] bool is_bidi_control_codepoint(wchar_t c) noexcept {
    return (c >= 0x202A && c <= 0x202E) || c == 0x2066 || c == 0x2067 || c == 0x2068 || c == 0x2069;
}

[[nodiscard]] bool needs_problem_char_replacement(std::wstring_view text, std::size_t pos) noexcept {
    const wchar_t c = text[pos];
    if (c == L'<' && allowed_br_tag_size(text, pos) != 0) {
        return false;
    }
    if (c == L'\r' || c == L'\n' || c == L'\t') {
        return true;
    }
    if (is_control_codepoint(c)) {
        return true;
    }
    if (is_bidi_control_codepoint(c)) {
        return true;
    }

    switch (c) {
        case L'&':
        case L'<':
        case L'>':
        case L'"':
        case L'\'':
        case L'\\':
        case L'$':
        case L'`':
            return true;
        default:
            return false;
    }
}

bool append_allowed_br_tag(std::wstring& out, std::wstring_view input, std::size_t& pos) {
    const std::size_t tag_size = allowed_br_tag_size(input, pos);
    if (tag_size == 0) {
        return false;
    }

    out.append(input.data() + pos, tag_size);
    pos += tag_size;
    return true;
}

void append_sanitized_char(std::wstring& out, std::wstring_view input, std::size_t& pos) {
    const wchar_t c = input[pos];

    if (c == L'<' && append_allowed_br_tag(out, input, pos)) {
        return;
    }

    if (c == L'\r') {
        if (pos + 1 < input.size() && input[pos + 1] == L'\n') {
            ++pos;
        }
        out += L"<br>";
        ++pos;
        return;
    }
    if (c == L'\n') {
        out += L"<br>";
        ++pos;
        return;
    }
    if (c == L'\t') {
        out += L"    ";
        ++pos;
        return;
    }

    if (is_control_codepoint(c) || is_bidi_control_codepoint(c)) {
        ++pos;
        return;
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
        case L'\\':
            out += L"&#92;";
            break;
        case L'$':
            out += L"&#36;";
            break;
        case L'`':
            out += L"&#96;";
            break;
        default:
            out.push_back(c);
            break;
    }
    ++pos;
}

[[nodiscard]] std::uint32_t decode_next_utf8(std::string_view input, std::size_t& pos, std::string_view field_name) {
    const auto b0 = static_cast<unsigned char>(input[pos]);
    if (b0 <= 0x7Fu) {
        ++pos;
        return b0;
    }

    std::size_t width = 0;
    std::uint32_t codepoint = 0;
    unsigned char min_second = 0x80u;
    unsigned char max_second = 0xBFu;

    if (b0 >= 0xC2u && b0 <= 0xDFu) {
        width = 2;
        codepoint = static_cast<std::uint32_t>(b0 & 0x1Fu);
    } else if (b0 >= 0xE0u && b0 <= 0xEFu) {
        width = 3;
        codepoint = static_cast<std::uint32_t>(b0 & 0x0Fu);
        if (b0 == 0xE0u) {
            min_second = 0xA0u;
        } else if (b0 == 0xEDu) {
            max_second = 0x9Fu;
        }
    } else if (b0 >= 0xF0u && b0 <= 0xF4u) {
        width = 4;
        codepoint = static_cast<std::uint32_t>(b0 & 0x07u);
        if (b0 == 0xF0u) {
            min_second = 0x90u;
        } else if (b0 == 0xF4u) {
            max_second = 0x8Fu;
        }
    } else {
        throw_invalid_utf8(field_name);
    }

    if (width > input.size() - pos) {
        throw_invalid_utf8(field_name);
    }

    for (std::size_t i = 1; i < width; ++i) {
        const auto byte = static_cast<unsigned char>(input[pos + i]);
        if (!is_utf8_continuation_byte(byte)) {
            throw_invalid_utf8(field_name);
        }
        if (i == 1 && (byte < min_second || byte > max_second)) {
            throw_invalid_utf8(field_name);
        }
        codepoint = (codepoint << 6) | static_cast<std::uint32_t>(byte & 0x3Fu);
    }

    if (codepoint > 0x10FFFFu || (codepoint >= 0xD800u && codepoint <= 0xDFFFu) ||
        codepoint > static_cast<std::uint32_t>(std::numeric_limits<wchar_t>::max())) {
        throw_invalid_utf8(field_name);
    }

    pos += width;
    return codepoint;
}

[[nodiscard]] std::wstring decode_utf8_or_throw(std::string_view input, std::string_view field_name) {
    std::wstring out;
    out.reserve(input.size());

    for (std::size_t i = 0; i < input.size();) {
        const std::uint32_t codepoint = decode_next_utf8(input, i, field_name);
        out.push_back(static_cast<wchar_t>(codepoint));
    }

    return out;
}

// Terminal state for the signal handler; valid only while a TermiosGuard is active.
termios g_saved_termios{};
volatile std::sig_atomic_t g_termios_saved = 0;

extern "C" void restore_terminal_and_reraise(int sig) {
    if (g_termios_saved) {
        ::tcsetattr(STDIN_FILENO, TCSANOW, &g_saved_termios);
    }
    // SA_RESETHAND has restored the default action; it runs once this handler returns.
    ::raise(sig);
}

// Switches the terminal to non-canonical, no-echo mode so a line is not capped at
// the kernel's 4096-byte canonical limit. The read loop then does its own echo and
// line editing, and the original mode is restored on scope exit or a fatal signal.
struct TermiosGuard {
    static constexpr std::array FATAL_SIGNALS{SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGXCPU};

    termios old{};
    bool active = false;
    std::array<struct sigaction, FATAL_SIGNALS.size()> old_actions{};
    std::array<bool, FATAL_SIGNALS.size()> handler_installed{};

    TermiosGuard() {
        if (!::isatty(STDIN_FILENO) || ::tcgetattr(STDIN_FILENO, &old) != 0) {
            return;
        }

        g_saved_termios = old;
        g_termios_saved = 1;
        install_signal_handlers();

        termios raw = old;
        raw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        // TCSADRAIN, not TCSAFLUSH: keep type-ahead and pasted input that is still queued.
        if (::tcsetattr(STDIN_FILENO, TCSADRAIN, &raw) == 0) {
            active = true;
        } else {
            restore_signal_handlers();
            g_termios_saved = 0;
        }
    }

    TermiosGuard(const TermiosGuard&) = delete;
    TermiosGuard& operator=(const TermiosGuard&) = delete;

    ~TermiosGuard() {
        if (active) {
            ::tcsetattr(STDIN_FILENO, TCSADRAIN, &old);
            restore_signal_handlers();
            g_termios_saved = 0;
        }
    }

    // True if `ch` is the terminal's configured special character for `index` (e.g. VERASE).
    [[nodiscard]] bool is_special_char(wchar_t ch, std::size_t index) const noexcept {
        const cc_t special = old.c_cc[index];
        return special != _POSIX_VDISABLE && ch == static_cast<wchar_t>(special);
    }

private:
    void install_signal_handlers() noexcept {
        for (auto [signal_number, old_action, installed] :
             std::views::zip(FATAL_SIGNALS, old_actions, handler_installed)) {
            struct sigaction current{};
            if (::sigaction(signal_number, nullptr, &current) != 0 || current.sa_handler == SIG_IGN) {
                continue;
            }
            struct sigaction action{};
            action.sa_handler = restore_terminal_and_reraise;
            action.sa_flags = SA_RESETHAND;
            ::sigemptyset(&action.sa_mask);
            installed = (::sigaction(signal_number, &action, &old_action) == 0);
        }
    }

    void restore_signal_handlers() noexcept {
        for (auto [signal_number, old_action, installed] :
             std::views::zip(FATAL_SIGNALS, old_actions, handler_installed)) {
            if (installed) {
                ::sigaction(signal_number, &old_action, nullptr);
                installed = false;
            }
        }
    }
};

void echo_to_terminal(std::string_view bytes) noexcept {
    std::fwrite(bytes.data(), 1, bytes.size(), stdout);
    std::fflush(stdout);
}

[[nodiscard]] std::wstring read_utf8_line_from_stdin(
    std::size_t max_chars,
    std::size_t max_utf8_bytes,
    std::string_view field_name) {
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

[[nodiscard]] std::wstring read_wide_line_from_tty(
    std::size_t max_chars,
    std::size_t max_utf8_bytes,
    std::string_view field_name) {
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

    // Width in terminal columns, so erasing rubs out exactly what was echoed.
    auto display_width = [](wchar_t ch) static {
        return (ch == L'\t') ? 1 : std::max(::wcwidth(ch), 0);
    };

    auto erase_last = [&] {
        if (s.empty()) {
            return;
        }
        const wchar_t last = s.back();
        s.pop_back();
        used_bytes -= utf8_encoded_bytes(last, field_name);
        for (int i = display_width(last); i > 0; --i) {
            echo_to_terminal("\b \b");
        }
    };

    auto append_and_echo = [&](wchar_t ch) {
        append_checked(ch);
        if (ch == L'\t') {
            echo_to_terminal(" ");
            return;
        }
        std::string encoded;
        append_utf8(encoded, static_cast<std::uint32_t>(ch));
        echo_to_terminal(encoded);
    };

    enum class Escape { None, Started, Csi, Ss3 };
    Escape escape = Escape::None;

    TermiosGuard guard;
    wchar_t ch;
    while (std::wcin.get(ch)) {
        if (!guard.active) {
            // Canonical mode: the kernel has already done the echo and line editing.
            if (ch == L'\n') {
                break;
            }
            append_checked(ch);
            continue;
        }

        if (ch == L'\n') {
            echo_to_terminal("\n");
            break;
        }

        // Swallow terminal escape sequences (arrow keys, Home/End, ...) instead of embedding them.
        if (escape == Escape::Csi) {
            if (ch >= 0x40 && ch <= 0x7E) {
                escape = Escape::None;
            }
            continue;
        }
        if (escape == Escape::Ss3) {
            escape = Escape::None;
            continue;
        }
        if (escape == Escape::Started) {
            escape = Escape::None;
            if (ch == L'[') {
                escape = Escape::Csi;
                continue;
            }
            if (ch == L'O') {
                escape = Escape::Ss3;
                continue;
            }
        }
        if (ch == 0x1B) {
            escape = Escape::Started;
            continue;
        }

        if (guard.is_special_char(ch, VERASE) || ch == 0x08 || ch == 0x7F) {
            erase_last();
            continue;
        }
        if (guard.is_special_char(ch, VKILL)) {
            while (!s.empty()) {
                erase_last();
            }
            continue;
        }
        if (guard.is_special_char(ch, VEOF)) {
            if (s.empty()) {
                echo_to_terminal("\n");
                break;
            }
            continue;
        }
        if (ch != L'\t' && is_control_codepoint(ch)) {
            continue;
        }

        append_and_echo(ch);
    }

    // getwc() reports an undecodable byte sequence as end-of-file with the stream error flag set.
    if (std::ferror(stdin)) {
        if (errno == EILSEQ) {
            throw_invalid_utf8(field_name);
        }
        throw std::runtime_error(std::format("{} Error: Failed to read input.", field_name));
    }
    if (std::wcin.bad()) {
        throw std::runtime_error(std::format("{} Error: Failed to read input.", field_name));
    }
    if (std::wcin.fail() && !std::wcin.eof()) {
        throw std::runtime_error(std::format("{} Error: Input contains invalid UTF-8 data.", field_name));
    }
    return s;
}
}  // namespace

namespace imgprmt::text {
bool has_valid_filename(const std::filesystem::path& p) {
    if (p.empty()) {
        return false;
    }

    std::string filename = p.filename().string();
    if (filename.empty()) {
        return false;
    }

    auto validChar = [](unsigned char c) static {
        return is_ascii_alnum(c) || c == '.' || c == '-' || c == '_' || c == '@' || c == '%';
    };
    return std::ranges::all_of(filename, validChar);
}

bool has_file_extension(const std::filesystem::path& p, std::initializer_list<std::string_view> exts) {
    auto e = p.extension().string();
    return std::ranges::any_of(exts, [&e](std::string_view ext) {
        if (ext.size() != e.size()) {
            return false;
        }
        return std::ranges::equal(ext, e, [](char a, char b) static {
            return ascii_lower(static_cast<unsigned char>(a)) == ascii_lower(static_cast<unsigned char>(b));
        });
    });
}

void replace_problem_chars(std::wstring& s) {
    const std::wstring_view input{s};
    std::size_t first_replacement = 0;
    while (first_replacement < input.size()) {
        if (input[first_replacement] == L'<') {
            const std::size_t br_tag_size = allowed_br_tag_size(input, first_replacement);
            if (br_tag_size != 0) {
                first_replacement += br_tag_size;
                continue;
            }
        }
        if (needs_problem_char_replacement(input, first_replacement)) {
            break;
        }
        ++first_replacement;
    }
    if (first_replacement == input.size()) {
        return;
    }

    std::wstring out;
    out.reserve(s.size() + 16);
    out.append(input.data(), first_replacement);

    for (std::size_t i = first_replacement; i < input.size();) {
        append_sanitized_char(out, input, i);
    }

    s.swap(out);
}

std::size_t calculate_default_prompt_budget(std::size_t url_bytes) {
    constexpr std::size_t DEFAULT_PROMPT_BUDGET = 57140;
    if (url_bytes >= DEFAULT_PROMPT_BUDGET) {
        throw std::runtime_error("Link Error: URL length leaves no room for prompt content.");
    }
    return DEFAULT_PROMPT_BUDGET - url_bytes;
}

std::size_t calculate_twitter_prompt_budget(std::size_t url_bytes) noexcept {
    constexpr std::size_t TWITTER_PROMPT_BUDGET = 1845;
    return (url_bytes < TWITTER_PROMPT_BUDGET) ? (TWITTER_PROMPT_BUDGET - url_bytes) : 0;
}

void enforce_prompt_size_limit(std::string_view utf8_prompt, std::size_t max_bytes) {
    if (utf8_prompt.size() > max_bytes) {
        throw std::runtime_error(std::format(
            "Prompt Error: Prompt exceeds maximum allowed size ({} bytes) for this URL.",
            max_bytes));
    }
}

bool force_utf8_locale() {
    // The C locale drives getwc() behind std::wcin. Do not re-apply the environment's
    // locale (std::locale("")) afterwards: it may not be UTF-8, or may not exist at all.
    return std::setlocale(LC_ALL, "C.UTF-8") || std::setlocale(LC_ALL, "en_US.UTF-8") || std::setlocale(LC_ALL, "UTF-8");
}

std::string convert_string(const std::wstring& wide) {
    std::string out;
    out.reserve(wide.size());

    using UnsignedWchar = std::make_unsigned_t<wchar_t>;
    for (wchar_t ch : wide) {
        const auto cp = static_cast<std::uint32_t>(static_cast<UnsignedWchar>(ch));
        if (cp > 0x10FFFFu || (cp >= 0xD800u && cp <= 0xDFFFu)) {
            throw std::runtime_error("Conversion to UTF-8 failed.");
        }
        append_utf8(out, cp);
    }
    return out;
}

std::wstring read_long_line_from_tty(std::size_t max_chars, std::size_t max_utf8_bytes, std::string_view field_name) {
    if (max_chars == 0) {
        throw std::runtime_error(std::format("{} Error: Maximum length is zero for this input.", field_name));
    }
    if (max_utf8_bytes == 0) {
        throw std::runtime_error(std::format("{} Error: Maximum byte length is zero for this input.", field_name));
    }

    if (!::isatty(STDIN_FILENO)) {
        return read_utf8_line_from_stdin(max_chars, max_utf8_bytes, field_name);
    }

    return read_wide_line_from_tty(max_chars, max_utf8_bytes, field_name);
}
}  // namespace imgprmt::text
