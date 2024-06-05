void replaceProblemChars(std::wstring& str) {

	constexpr wchar_t Tag[]{ 39, 13, 10 };

	for (int i = static_cast<int>(str.length()); i != -1; i--) {
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