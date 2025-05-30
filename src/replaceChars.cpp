#include "replaceChars.h"
#include <array>

void replaceProblemChars(std::wstring& str) {
	constexpr std::array<wchar_t, 3> Tag { 39, 13, 10 };
	   for (int i = static_cast<int>(str.length()) - 1; i >= 0; i--) {
		wchar_t c = str[i];
		for (int x = 0; x != 3; x++) {
			if (c == Tag[x]) {
				std::string html_entity = "&#" + std::to_string(Tag[x]) + ';';
				str.erase(str.begin() + i);
				str.insert(str.begin() + i, html_entity.begin(), html_entity.end());
			}
		}
	}
}
