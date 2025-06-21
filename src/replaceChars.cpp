#include "replaceChars.h"
#include <array>

void replaceProblemChars(std::wstring& str) {
    constexpr uint8_t ARR_SIZE = 4;
    constexpr std::array<wchar_t, ARR_SIZE> Tag{ 96, 39, 13, 10 }; 

    for (int i = static_cast<int>(str.length()) - 1; i >= 0; --i) {
        wchar_t c = str[i];
        for (wchar_t tagChar : Tag) {
            if (c == tagChar) {
                std::wstring html_entity = L"&#" + std::to_wstring(tagChar) + L";";
                str.erase(str.begin() + i);
                str.insert(str.begin() + i, html_entity.begin(), html_entity.end());
                break; 
            }
        }
    }
}
