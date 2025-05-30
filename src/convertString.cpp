#include <stdexcept>

#ifdef _WIN32
    #define NOMINMAX
    #include <windows.h>
#else
    #include <cwchar>
#endif

std::string convertString(const std::wstring& wide) {
    #ifdef _WIN32
        if (wide.empty()) return {};

        int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (size <= 0) throw std::runtime_error("WideCharToMultiByte failed.");

        std::string convertedString(size - 1, 0); 
        WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, convertedString.data(), size, nullptr, nullptr);
        return convertedString;

    #else
        std::mbstate_t state{};
        const wchar_t* src = wide.data();
        size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
        if (len == static_cast<size_t>(-1)) throw std::runtime_error("Conversion to UTF-8 failed.");

        std::string convertedString(len, 0);
        src = wide.data(); // reset pointer
        std::wcsrtombs(convertedString.data(), &src, len, &state);
        return convertedString;
    #endif
}
