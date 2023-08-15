//	imgprmt v1.1 (Windows Edition). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <io.h>
#include <vector>

// Writes updated size values (e.g. iCC Profile length) into relevant vector index locations. Overwrites previous size values.
class ValueUpdater {
public:
	void Value(std::vector<unsigned char>& vect, int valueInsertIndex, const size_t VALUE, int bits) {
		while (bits) {
			vect[valueInsertIndex++] = (VALUE >> (bits -= 8)) & 0xff;
		}
	}
} *update;

void replaceSpecialChars(std::wstring& str) {

	// For certain characters to display correctly within the html prompt/description, we need to find and replace them with
	// the corrent html entity code. For example, for the word "café", the é (Latin small letter e with Acute character),
	// is replaced with the html entity code "&#233"; Note, this is not an exhaustive list.
	
	// Current wide character values that are replaced with the html entity codes.
	// Character value 39 (apostrophe) is not a wide character, but we still need to replace it with a html entity code
	// as it will break the html page if inserted in its raw form.
	wchar_t Tag[117]{
		11013, 11014, 11015, 11157, 10145, 8592, 8593, 8594, 8595, 8596, 8597, 8217, 8211, 8212, 960, 351, 324, 322,287, 255, 254, 253, 252, 251, 250,
		249, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228,
		227, 226, 225, 224, 223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208, 207, 206,
		205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192, 191, 190, 189, 188, 187, 186, 185, 184,
		183, 182, 181, 180, 179, 178, 177, 176, 175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161, 144, 142, 39
	};

	// Check each character of the w_prompt (str) string for wide characters and replace them with the html entity string.
	for (std::size_t i = str.length(); i != -1; i--) {
		wchar_t c = str[i];
		for (int x = 0; x != 117; x++) {
			if (c == Tag[x]) {
				std::string htmlEntity = "&#" + std::to_string(Tag[x]);
				str.erase(str.begin() + i);
				str.insert(str.begin() + i, htmlEntity.begin(), htmlEntity.end());
			}
		}
	}
}

void openFiles(char* []);
void displayInfo();

int main(int argc, char** argv) {

	if (argc == 2 && std::string(argv[1]) == "--info") {
		displayInfo();
	}
	else if (argc > 1 && argc < 3) {
		openFiles(argv);
	}
	else {
		std::cout << "\nUsage:\timgprmt <jpg-image>\n\timgprmt --info\n\n";
	}
	return 0;
}

void openFiles(char* argv[]) {

	const std::string IMAGE_NAME = argv[1];

	std::ifstream readImage(IMAGE_NAME, std::ios::binary);

	if (!readImage) {
		std::cerr << "\nRead Error: Unable to open/read file: \"" + IMAGE_NAME + "\"\n\n";
		std::exit(EXIT_FAILURE);
	}

	// This vector contains our JPG header with a basic ICC Profile and HTML page,
	// currently without the users image prompt & url, which is inserted later.
	std::vector<unsigned char>ProfileVec = {
	0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0xFF, 0xE2, 0x0F, 0xF8,
	0x49, 0x43, 0x43, 0x5F, 0x50, 0x52, 0x4F, 0x46, 0x49, 0x4C, 0x45, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x0F, 0xE8, 0x3C, 0x21, 0x2D, 0x2D, 0x04, 0x30,
	0x00, 0x00, 0x6D, 0x6E, 0x74, 0x72, 0x52, 0x47, 0x42, 0x20, 0x58, 0x59,
	0x5A, 0x20, 0x07, 0xE0, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x61, 0x63, 0x73, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
	0xF6, 0xD6, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD3, 0x2D, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x09, 0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x00, 0xE4, 0x00, 0x00,
	0x00, 0x24, 0x72, 0x58, 0x59, 0x5A, 0x00, 0x00, 0x01, 0x08, 0x00, 0x00,
	0x00, 0x14, 0x67, 0x58, 0x59, 0x5A, 0x00, 0x00, 0x01, 0x1C, 0x00, 0x00,
	0x00, 0x14, 0x62, 0x58, 0x59, 0x5A, 0x00, 0x00, 0x01, 0x30, 0x00, 0x00,
	0x00, 0x14, 0x77, 0x74, 0x70, 0x74, 0x00, 0x00, 0x01, 0x44, 0x00, 0x00,
	0x00, 0x14, 0x72, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x58, 0x00, 0x00,
	0x00, 0x28, 0x67, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x58, 0x00, 0x00,
	0x00, 0x28, 0x62, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x58, 0x00, 0x00,
	0x00, 0x28, 0x6D, 0x6C, 0x75, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x0C, 0x65, 0x6E, 0x55, 0x53, 0x00, 0x00,
	0x00, 0x08, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x73, 0x00, 0x52, 0x00, 0x47,
	0x00, 0x42, 0x58, 0x59, 0x5A, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x6F, 0xA2, 0x00, 0x00, 0x38, 0xF5, 0x00, 0x00, 0x03, 0x90, 0x58, 0x59,
	0x5A, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x99, 0x00, 0x00,
	0xB7, 0x85, 0x00, 0x00, 0x18, 0xDA, 0x58, 0x59, 0x5A, 0x20, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x24, 0xA0, 0x00, 0x00, 0x0F, 0x84, 0x00, 0x00,
	0xB6, 0xCF, 0x58, 0x59, 0x5A, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xF6, 0xD6, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD3, 0x2D, 0x70, 0x61,
	0x72, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02,
	0x66, 0x66, 0x00, 0x00, 0xF2, 0xA7, 0x00, 0x00, 0x0D, 0x59, 0x00, 0x00,
	0x13, 0xD0, 0x00, 0x00, 0x0A, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x2D, 0x2D, 0x3E, 0x3C, 0x21, 0x44, 0x4F, 0x43, 0x54, 0x59,
	0x50, 0x45, 0x20, 0x68, 0x74, 0x6D, 0x6C, 0x3E, 0x3C, 0x68, 0x74, 0x6D,
	0x6C, 0x20, 0x6C, 0x61, 0x6E, 0x67, 0x3D, 0x22, 0x65, 0x6E, 0x22, 0x3E,
	0x3C, 0x68, 0x65, 0x61, 0x64, 0x3E, 0x3C, 0x74, 0x69, 0x74, 0x6C, 0x65,
	0x3E, 0x49, 0x6D, 0x61, 0x67, 0x65, 0x20, 0x50, 0x72, 0x6F, 0x6D, 0x70,
	0x74, 0x20, 0x28, 0x69, 0x6D, 0x67, 0x70, 0x72, 0x6D, 0x74, 0x29, 0x20,
	0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 0x64, 0x20, 0x62, 0x79, 0x20, 0x4E,
	0x69, 0x63, 0x68, 0x6F, 0x6C, 0x61, 0x73, 0x20, 0x43, 0x6C, 0x65, 0x61,
	0x73, 0x62, 0x79, 0x3C, 0x2F, 0x74, 0x69, 0x74, 0x6C, 0x65, 0x3E, 0x3C,
	0x6D, 0x65, 0x74, 0x61, 0x20, 0x6E, 0x61, 0x6D, 0x65, 0x3D, 0x22, 0x76,
	0x69, 0x65, 0x77, 0x70, 0x6F, 0x72, 0x74, 0x22, 0x20, 0x63, 0x6F, 0x6E,
	0x74, 0x65, 0x6E, 0x74, 0x3D, 0x22, 0x77, 0x69, 0x64, 0x74, 0x68, 0x3D,
	0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x77, 0x69, 0x64, 0x74, 0x68,
	0x2C, 0x20, 0x69, 0x6E, 0x69, 0x74, 0x69, 0x61, 0x6C, 0x2D, 0x73, 0x63,
	0x61, 0x6C, 0x65, 0x3D, 0x31, 0x22, 0x3E, 0x3C, 0x6C, 0x69, 0x6E, 0x6B,
	0x20, 0x68, 0x72, 0x65, 0x66, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x73,
	0x3A, 0x2F, 0x2F, 0x66, 0x6F, 0x6E, 0x74, 0x73, 0x2E, 0x67, 0x6F, 0x6F,
	0x67, 0x6C, 0x65, 0x61, 0x70, 0x69, 0x73, 0x2E, 0x63, 0x6F, 0x6D, 0x2F,
	0x63, 0x73, 0x73, 0x3F, 0x66, 0x61, 0x6D, 0x69, 0x6C, 0x79, 0x3D, 0x4C,
	0x61, 0x74, 0x6F, 0x22, 0x20, 0x72, 0x65, 0x6C, 0x3D, 0x22, 0x73, 0x74,
	0x79, 0x6C, 0x65, 0x73, 0x68, 0x65, 0x65, 0x74, 0x22, 0x3E, 0x3C, 0x73,
	0x63, 0x72, 0x69, 0x70, 0x74, 0x20, 0x74, 0x79, 0x70, 0x65, 0x3D, 0x22,
	0x74, 0x65, 0x78, 0x74, 0x2F, 0x6A, 0x61, 0x76, 0x61, 0x73, 0x63, 0x72,
	0x69, 0x70, 0x74, 0x22, 0x3E, 0x64, 0x6F, 0x63, 0x75, 0x6D, 0x65, 0x6E,
	0x74, 0x2E, 0x62, 0x6F, 0x64, 0x79, 0x2E, 0x69, 0x6E, 0x6E, 0x65, 0x72,
	0x48, 0x54, 0x4D, 0x4C, 0x3D, 0x27, 0x27, 0x3B, 0x76, 0x61, 0x72, 0x20,
	0x70, 0x61, 0x74, 0x68, 0x3D, 0x77, 0x69, 0x6E, 0x64, 0x6F, 0x77, 0x2E,
	0x6C, 0x6F, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x2E, 0x70, 0x61, 0x74,
	0x68, 0x6E, 0x61, 0x6D, 0x65, 0x3B, 0x76, 0x61, 0x72, 0x20, 0x66, 0x6E,
	0x61, 0x6D, 0x65, 0x3D, 0x70, 0x61, 0x74, 0x68, 0x2E, 0x73, 0x70, 0x6C,
	0x69, 0x74, 0x28, 0x22, 0x2F, 0x22, 0x29, 0x2E, 0x70, 0x6F, 0x70, 0x28,
	0x29, 0x3B, 0x64, 0x6F, 0x63, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x2E, 0x62,
	0x6F, 0x64, 0x79, 0x2E, 0x69, 0x6E, 0x6E, 0x65, 0x72, 0x48, 0x54, 0x4D,
	0x4C, 0x3D, 0x27, 0x3C, 0x62, 0x6F, 0x64, 0x79, 0x3E, 0x3C, 0x73, 0x74,
	0x79, 0x6C, 0x65, 0x3E, 0x62, 0x6F, 0x64, 0x79, 0x7B, 0x62, 0x61, 0x63,
	0x6B, 0x67, 0x72, 0x6F, 0x75, 0x6E, 0x64, 0x2D, 0x63, 0x6F, 0x6C, 0x6F,
	0x72, 0x3A, 0x23, 0x30, 0x46, 0x30, 0x46, 0x30, 0x46, 0x3B, 0x7D, 0x62,
	0x75, 0x74, 0x74, 0x6F, 0x6E, 0x7B, 0x62, 0x61, 0x63, 0x6B, 0x67, 0x72,
	0x6F, 0x75, 0x6E, 0x64, 0x2D, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3A, 0x62,
	0x6C, 0x61, 0x63, 0x6B, 0x3B, 0x62, 0x6F, 0x72, 0x64, 0x65, 0x72, 0x2D,
	0x72, 0x61, 0x64, 0x69, 0x75, 0x73, 0x3A, 0x31, 0x30, 0x70, 0x78, 0x3B,
	0x62, 0x6F, 0x72, 0x64, 0x65, 0x72, 0x3A, 0x32, 0x70, 0x78, 0x20, 0x73,
	0x6F, 0x6C, 0x69, 0x64, 0x20, 0x70, 0x75, 0x72, 0x70, 0x6C, 0x65, 0x3B,
	0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3A, 0x64, 0x61, 0x72, 0x6B, 0x6F, 0x72,
	0x61, 0x6E, 0x67, 0x65, 0x3B, 0x70, 0x61, 0x64, 0x64, 0x69, 0x6E, 0x67,
	0x3A, 0x36, 0x70, 0x78, 0x20, 0x31, 0x32, 0x70, 0x78, 0x3B, 0x74, 0x65,
	0x78, 0x74, 0x2D, 0x64, 0x65, 0x63, 0x6F, 0x72, 0x61, 0x74, 0x69, 0x6F,
	0x6E, 0x2D, 0x73, 0x74, 0x79, 0x6C, 0x65, 0x3A, 0x73, 0x6F, 0x6C, 0x69,
	0x64, 0x3B, 0x74, 0x65, 0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E,
	0x3A, 0x63, 0x65, 0x6E, 0x74, 0x65, 0x72, 0x3B, 0x64, 0x69, 0x73, 0x70,
	0x6C, 0x61, 0x79, 0x3A, 0x69, 0x6E, 0x6C, 0x69, 0x6E, 0x65, 0x2D, 0x62,
	0x6C, 0x6F, 0x63, 0x6B, 0x3B, 0x66, 0x6F, 0x6E, 0x74, 0x2D, 0x73, 0x69,
	0x7A, 0x65, 0x3A, 0x31, 0x32, 0x70, 0x78, 0x3B, 0x6D, 0x61, 0x72, 0x67,
	0x69, 0x6E, 0x3A, 0x34, 0x70, 0x78, 0x20, 0x32, 0x70, 0x78, 0x3B, 0x74,
	0x72, 0x61, 0x6E, 0x73, 0x69, 0x74, 0x69, 0x6F, 0x6E, 0x2D, 0x64, 0x75,
	0x72, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x3A, 0x30, 0x2E, 0x34, 0x73, 0x3B,
	0x63, 0x75, 0x72, 0x73, 0x6F, 0x72, 0x3A, 0x70, 0x6F, 0x69, 0x6E, 0x74,
	0x65, 0x72, 0x3B, 0x7D, 0x62, 0x75, 0x74, 0x74, 0x6F, 0x6E, 0x3A, 0x68,
	0x6F, 0x76, 0x65, 0x72, 0x3A, 0x65, 0x6E, 0x61, 0x62, 0x6C, 0x65, 0x64,
	0x7B, 0x62, 0x61, 0x63, 0x6B, 0x67, 0x72, 0x6F, 0x75, 0x6E, 0x64, 0x2D,
	0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3A, 0x62, 0x6C, 0x75, 0x65, 0x3B, 0x63,
	0x6F, 0x6C, 0x6F, 0x72, 0x3A, 0x23, 0x66, 0x66, 0x66, 0x3B, 0x7D, 0x62,
	0x75, 0x74, 0x74, 0x6F, 0x6E, 0x3A, 0x3A, 0x61, 0x66, 0x74, 0x65, 0x72,
	0x7B, 0x63, 0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x3A, 0x22, 0x50, 0x52,
	0x4F, 0x4D, 0x50, 0x54, 0x22, 0x3B, 0x7D, 0x62, 0x75, 0x74, 0x74, 0x6F,
	0x6E, 0x3A, 0x68, 0x6F, 0x76, 0x65, 0x72, 0x3A, 0x3A, 0x61, 0x66, 0x74,
	0x65, 0x72, 0x7B, 0x63, 0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 0x3A, 0x22,
	0x43, 0x4F, 0x50, 0x59, 0x22, 0x3B, 0x7D, 0x62, 0x72, 0x7B, 0x6C, 0x69,
	0x6E, 0x65, 0x2D, 0x68, 0x65, 0x69, 0x67, 0x68, 0x74, 0x3A, 0x30, 0x2E,
	0x39, 0x65, 0x6D, 0x3B, 0x7D, 0x68, 0x31, 0x7B, 0x63, 0x6F, 0x6C, 0x6F,
	0x72, 0x3A, 0x77, 0x68, 0x69, 0x74, 0x65, 0x3B, 0x62, 0x61, 0x63, 0x6B,
	0x67, 0x72, 0x6F, 0x75, 0x6E, 0x64, 0x3A, 0x6C, 0x69, 0x6E, 0x65, 0x61,
	0x72, 0x2D, 0x67, 0x72, 0x61, 0x64, 0x69, 0x65, 0x6E, 0x74, 0x28, 0x74,
	0x6F, 0x20, 0x72, 0x69, 0x67, 0x68, 0x74, 0x2C, 0x72, 0x65, 0x64, 0x2C,
	0x67, 0x72, 0x65, 0x65, 0x6E, 0x2C, 0x62, 0x6C, 0x75, 0x65, 0x2C, 0x79,
	0x65, 0x6C, 0x6C, 0x6F, 0x77, 0x29, 0x3B, 0x62, 0x6F, 0x72, 0x64, 0x65,
	0x72, 0x2D, 0x72, 0x61, 0x64, 0x69, 0x75, 0x73, 0x3A, 0x31, 0x32, 0x70,
	0x78, 0x3B, 0x70, 0x61, 0x64, 0x64, 0x69, 0x6E, 0x67, 0x3A, 0x33, 0x70,
	0x78, 0x3B, 0x74, 0x65, 0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E,
	0x3A, 0x63, 0x65, 0x6E, 0x74, 0x65, 0x72, 0x3B, 0x66, 0x6F, 0x6E, 0x74,
	0x2D, 0x73, 0x69, 0x7A, 0x65, 0x3A, 0x33, 0x36, 0x70, 0x78, 0x3B, 0x6D,
	0x61, 0x72, 0x67, 0x69, 0x6E, 0x2D, 0x62, 0x6C, 0x6F, 0x63, 0x6B, 0x2D,
	0x65, 0x6E, 0x64, 0x3A, 0x30, 0x2E, 0x31, 0x65, 0x6D, 0x3B, 0x6D, 0x61,
	0x72, 0x67, 0x69, 0x6E, 0x2D, 0x62, 0x6C, 0x6F, 0x63, 0x6B, 0x2D, 0x73,
	0x74, 0x61, 0x72, 0x74, 0x3A, 0x30, 0x2E, 0x33, 0x65, 0x6D, 0x3B, 0x3B,
	0x7D, 0x68, 0x33, 0x7B, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3A, 0x6C, 0x69,
	0x67, 0x68, 0x74, 0x63, 0x79, 0x61, 0x6E, 0x3B, 0x66, 0x6F, 0x6E, 0x74,
	0x2D, 0x66, 0x61, 0x6D, 0x69, 0x6C, 0x79, 0x3A, 0x4C, 0x61, 0x74, 0x6F,
	0x3B, 0x66, 0x6F, 0x6E, 0x74, 0x2D, 0x73, 0x69, 0x7A, 0x65, 0x3A, 0x32,
	0x34, 0x70, 0x78, 0x3B, 0x74, 0x65, 0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69,
	0x67, 0x6E, 0x3A, 0x73, 0x74, 0x61, 0x72, 0x74, 0x3B, 0x74, 0x65, 0x78,
	0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E, 0x3A, 0x65, 0x6E, 0x64, 0x3B,
	0x74, 0x65, 0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E, 0x3A, 0x6C,
	0x65, 0x66, 0x74, 0x3B, 0x74, 0x65, 0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69,
	0x67, 0x6E, 0x3A, 0x72, 0x69, 0x67, 0x68, 0x74, 0x3B, 0x74, 0x65, 0x78,
	0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E, 0x3A, 0x63, 0x65, 0x6E, 0x74,
	0x65, 0x72, 0x3B, 0x74, 0x65, 0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67,
	0x6E, 0x3A, 0x6A, 0x75, 0x73, 0x74, 0x69, 0x66, 0x79, 0x3B, 0x6D, 0x61,
	0x72, 0x67, 0x69, 0x6E, 0x3A, 0x30, 0x2E, 0x36, 0x65, 0x6D, 0x20, 0x31,
	0x31, 0x2E, 0x32, 0x65, 0x6D, 0x3B, 0x7D, 0x69, 0x6D, 0x67, 0x7B, 0x62,
	0x6F, 0x72, 0x64, 0x65, 0x72, 0x3A, 0x32, 0x70, 0x78, 0x20, 0x73, 0x6F,
	0x6C, 0x69, 0x64, 0x20, 0x64, 0x69, 0x6D, 0x67, 0x72, 0x65, 0x79, 0x3B,
	0x62, 0x6F, 0x72, 0x64, 0x65, 0x72, 0x2D, 0x72, 0x61, 0x64, 0x69, 0x75,
	0x73, 0x3A, 0x31, 0x34, 0x70, 0x78, 0x3B, 0x77, 0x69, 0x64, 0x74, 0x68,
	0x3A, 0x32, 0x37, 0x38, 0x70, 0x78, 0x3B, 0x7D, 0x68, 0x33, 0x3A, 0x68,
	0x6F, 0x76, 0x65, 0x72, 0x7B, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3A, 0x64,
	0x61, 0x72, 0x6B, 0x63, 0x79, 0x61, 0x6E, 0x3B, 0x7D, 0x69, 0x6D, 0x67,
	0x3A, 0x68, 0x6F, 0x76, 0x65, 0x72, 0x7B, 0x62, 0x6F, 0x72, 0x64, 0x65,
	0x72, 0x3A, 0x32, 0x70, 0x78, 0x20, 0x73, 0x6F, 0x6C, 0x69, 0x64, 0x20,
	0x44, 0x61, 0x72, 0x6B, 0x4F, 0x72, 0x61, 0x6E, 0x67, 0x65, 0x3B, 0x62,
	0x6F, 0x78, 0x2D, 0x73, 0x68, 0x61, 0x64, 0x6F, 0x77, 0x3A, 0x30, 0x70,
	0x78, 0x20, 0x30, 0x70, 0x78, 0x20, 0x36, 0x70, 0x78, 0x20, 0x36, 0x70,
	0x78, 0x20, 0x59, 0x65, 0x6C, 0x6C, 0x6F, 0x77, 0x3B, 0x7D, 0x68, 0x72,
	0x7B, 0x62, 0x6F, 0x72, 0x64, 0x65, 0x72, 0x3A, 0x32, 0x70, 0x78, 0x20,
	0x73, 0x6F, 0x6C, 0x69, 0x64, 0x20, 0x23, 0x34, 0x42, 0x30, 0x31, 0x35,
	0x30, 0x3B, 0x7D, 0x66, 0x6F, 0x6F, 0x74, 0x65, 0x72, 0x7B, 0x63, 0x6F,
	0x6C, 0x6F, 0x72, 0x3A, 0x77, 0x68, 0x69, 0x74, 0x65, 0x3B, 0x74, 0x65,
	0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E, 0x3A, 0x63, 0x65, 0x6E,
	0x74, 0x65, 0x72, 0x3B, 0x70, 0x61, 0x64, 0x64, 0x69, 0x6E, 0x67, 0x3A,
	0x31, 0x32, 0x70, 0x78, 0x3B, 0x66, 0x6F, 0x6E, 0x74, 0x2D, 0x73, 0x69,
	0x7A, 0x65, 0x3A, 0x31, 0x35, 0x70, 0x78, 0x3B, 0x62, 0x61, 0x63, 0x6B,
	0x67, 0x72, 0x6F, 0x75, 0x6E, 0x64, 0x3A, 0x6C, 0x69, 0x6E, 0x65, 0x61,
	0x72, 0x2D, 0x67, 0x72, 0x61, 0x64, 0x69, 0x65, 0x6E, 0x74, 0x28, 0x74,
	0x6F, 0x20, 0x6C, 0x65, 0x66, 0x74, 0x2C, 0x23, 0x31, 0x36, 0x32, 0x32,
	0x32, 0x36, 0x20, 0x30, 0x25, 0x2C, 0x23, 0x31, 0x36, 0x32, 0x32, 0x32,
	0x46, 0x20, 0x31, 0x30, 0x30, 0x25, 0x29, 0x3B, 0x62, 0x6F, 0x72, 0x64,
	0x65, 0x72, 0x2D, 0x72, 0x61, 0x64, 0x69, 0x75, 0x73, 0x3A, 0x31, 0x30,
	0x70, 0x78, 0x3B, 0x6D, 0x61, 0x72, 0x67, 0x69, 0x6E, 0x2D, 0x62, 0x6C,
	0x6F, 0x63, 0x6B, 0x2D, 0x65, 0x6E, 0x64, 0x3A, 0x30, 0x2E, 0x31, 0x65,
	0x6D, 0x3B, 0x6D, 0x61, 0x72, 0x67, 0x69, 0x6E, 0x2D, 0x62, 0x6C, 0x6F,
	0x63, 0x6B, 0x2D, 0x73, 0x74, 0x61, 0x72, 0x74, 0x3A, 0x30, 0x2E, 0x33,
	0x65, 0x6D, 0x3B, 0x7D, 0x61, 0x7B, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3A,
	0x77, 0x68, 0x69, 0x74, 0x65, 0x3B, 0x7D, 0x61, 0x3A, 0x68, 0x6F, 0x76,
	0x65, 0x72, 0x7B, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3A, 0x6F, 0x72, 0x61,
	0x6E, 0x67, 0x65, 0x3B, 0x7D, 0x40, 0x6D, 0x65, 0x64, 0x69, 0x61, 0x20,
	0x73, 0x63, 0x72, 0x65, 0x65, 0x6E, 0x20, 0x61, 0x6E, 0x64, 0x20, 0x28,
	0x6D, 0x61, 0x78, 0x2D, 0x77, 0x69, 0x64, 0x74, 0x68, 0x3A, 0x20, 0x34,
	0x31, 0x35, 0x70, 0x78, 0x29, 0x20, 0x7B, 0x68, 0x33, 0x20, 0x7B, 0x66,
	0x6F, 0x6E, 0x74, 0x2D, 0x73, 0x69, 0x7A, 0x65, 0x3A, 0x31, 0x39, 0x2E,
	0x37, 0x70, 0x78, 0x3B, 0x6D, 0x61, 0x72, 0x67, 0x69, 0x6E, 0x3A, 0x31,
	0x2E, 0x36, 0x65, 0x6D, 0x20, 0x30, 0x2E, 0x35, 0x65, 0x6D, 0x3B, 0x7D,
	0x68, 0x31, 0x7B, 0x66, 0x6F, 0x6E, 0x74, 0x2D, 0x73, 0x69, 0x7A, 0x65,
	0x3A, 0x32, 0x32, 0x2E, 0x38, 0x70, 0x78, 0x3B, 0x7D, 0x7D, 0x40, 0x6D,
	0x65, 0x64, 0x69, 0x61, 0x20, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6E, 0x20,
	0x61, 0x6E, 0x64, 0x20, 0x28, 0x6D, 0x61, 0x78, 0x2D, 0x77, 0x69, 0x64,
	0x74, 0x68, 0x3A, 0x39, 0x31, 0x36, 0x70, 0x78, 0x29, 0x20, 0x61, 0x6E,
	0x64, 0x20, 0x28, 0x6F, 0x72, 0x69, 0x65, 0x6E, 0x74, 0x61, 0x74, 0x69,
	0x6F, 0x6E, 0x3A, 0x6C, 0x61, 0x6E, 0x64, 0x73, 0x63, 0x61, 0x70, 0x65,
	0x29, 0x7B, 0x68, 0x33, 0x7B, 0x66, 0x6F, 0x6E, 0x74, 0x2D, 0x73, 0x69,
	0x7A, 0x65, 0x3A, 0x32, 0x30, 0x70, 0x78, 0x3B, 0x6D, 0x61, 0x72, 0x67,
	0x69, 0x6E, 0x3A, 0x31, 0x2E, 0x36, 0x65, 0x6D, 0x20, 0x35, 0x65, 0x6D,
	0x3B, 0x7D, 0x7D, 0x3C, 0x2F, 0x73, 0x74, 0x79, 0x6C, 0x65, 0x3E, 0x3C,
	0x68, 0x31, 0x20, 0x69, 0x64, 0x3D, 0x22, 0x42, 0x61, 0x72, 0x43, 0x6F,
	0x6C, 0x6F, 0x72, 0x22, 0x3E, 0x4D, 0x69, 0x64, 0x6A, 0x6F, 0x75, 0x72,
	0x6E, 0x65, 0x79, 0x20, 0x50, 0x72, 0x6F, 0x6D, 0x70, 0x74, 0x20, 0x53,
	0x68, 0x61, 0x72, 0x65, 0x3C, 0x2F, 0x68, 0x31, 0x3E, 0x3C, 0x68, 0x72,
	0x3E, 0x3C, 0x62, 0x72, 0x3E, 0x3C, 0x62, 0x72, 0x3E, 0x3C, 0x63, 0x65,
	0x6E, 0x74, 0x65, 0x72, 0x3E, 0x3C, 0x61, 0x20, 0x74, 0x61, 0x72, 0x67,
	0x65, 0x74, 0x3D, 0x22, 0x5F, 0x62, 0x6C, 0x61, 0x6E, 0x6B, 0x22, 0x20,
	0x68, 0x72, 0x65, 0x66, 0x3D, 0x22, 0x22, 0x3E, 0x3C, 0x69, 0x6D, 0x67,
	0x20, 0x73, 0x72, 0x63, 0x3D, 0x22, 0x27, 0x20, 0x2B, 0x20, 0x66, 0x6E,
	0x61, 0x6D, 0x65, 0x20, 0x2B, 0x20, 0x27, 0x22, 0x20, 0x61, 0x6C, 0x74,
	0x3D, 0x22, 0x49, 0x6D, 0x61, 0x67, 0x65, 0x20, 0x50, 0x72, 0x65, 0x76,
	0x69, 0x65, 0x77, 0x22, 0x3E, 0x3C, 0x2F, 0x61, 0x3E, 0x3C, 0x2F, 0x63,
	0x65, 0x6E, 0x74, 0x65, 0x72, 0x3E, 0x3C, 0x62, 0x72, 0x3E, 0x3C, 0x63,
	0x65, 0x6E, 0x74, 0x65, 0x72, 0x3E, 0x3C, 0x62, 0x75, 0x74, 0x74, 0x6F,
	0x6E, 0x20, 0x69, 0x64, 0x3D, 0x22, 0x63, 0x6F, 0x70, 0x79, 0x42, 0x75,
	0x74, 0x74, 0x6F, 0x6E, 0x22, 0x3E, 0x3C, 0x2F, 0x62, 0x75, 0x74, 0x74,
	0x6F, 0x6E, 0x3E, 0x3C, 0x2F, 0x63, 0x65, 0x6E, 0x74, 0x65, 0x72, 0x3E,
	0x3C, 0x68, 0x33, 0x20, 0x69, 0x64, 0x3D, 0x22, 0x50, 0x72, 0x6F, 0x6D,
	0x70, 0x74, 0x43, 0x6F, 0x70, 0x79, 0x22, 0x3E, 0x3C, 0x2F, 0x68, 0x33,
	0x3E, 0x3C, 0x62, 0x72, 0x3E, 0x27, 0x3C, 0x2F, 0x73, 0x63, 0x72, 0x69,
	0x70, 0x74, 0x3E, 0x3C, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x20, 0x74,
	0x79, 0x70, 0x65, 0x3D, 0x22, 0x74, 0x65, 0x78, 0x74, 0x2F, 0x6A, 0x61,
	0x76, 0x61, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x22, 0x3E, 0x66, 0x75,
	0x6E, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x63, 0x6F, 0x70, 0x79, 0x54,
	0x6F, 0x43, 0x6C, 0x69, 0x70, 0x62, 0x6F, 0x61, 0x72, 0x64, 0x28, 0x74,
	0x65, 0x78, 0x74, 0x29, 0x7B, 0x63, 0x6F, 0x6E, 0x73, 0x74, 0x20, 0x74,
	0x65, 0x6D, 0x70, 0x49, 0x6E, 0x70, 0x75, 0x74, 0x3D, 0x64, 0x6F, 0x63,
	0x75, 0x6D, 0x65, 0x6E, 0x74, 0x2E, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65,
	0x45, 0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x28, 0x22, 0x69, 0x6E, 0x70,
	0x75, 0x74, 0x22, 0x29, 0x3B, 0x74, 0x65, 0x6D, 0x70, 0x49, 0x6E, 0x70,
	0x75, 0x74, 0x2E, 0x73, 0x74, 0x79, 0x6C, 0x65, 0x2E, 0x70, 0x6F, 0x73,
	0x69, 0x74, 0x69, 0x6F, 0x6E, 0x3D, 0x22, 0x61, 0x62, 0x73, 0x6F, 0x6C,
	0x75, 0x74, 0x65, 0x22, 0x3B, 0x74, 0x65, 0x6D, 0x70, 0x49, 0x6E, 0x70,
	0x75, 0x74, 0x2E, 0x73, 0x74, 0x79, 0x6C, 0x65, 0x2E, 0x6C, 0x65, 0x66,
	0x74, 0x3D, 0x22, 0x2D, 0x39, 0x39, 0x39, 0x39, 0x70, 0x78, 0x22, 0x3B,
	0x74, 0x65, 0x6D, 0x70, 0x49, 0x6E, 0x70, 0x75, 0x74, 0x2E, 0x76, 0x61,
	0x6C, 0x75, 0x65, 0x3D, 0x74, 0x65, 0x78, 0x74, 0x3B, 0x64, 0x6F, 0x63,
	0x75, 0x6D, 0x65, 0x6E, 0x74, 0x2E, 0x62, 0x6F, 0x64, 0x79, 0x2E, 0x61,
	0x70, 0x70, 0x65, 0x6E, 0x64, 0x43, 0x68, 0x69, 0x6C, 0x64, 0x28, 0x74,
	0x65, 0x6D, 0x70, 0x49, 0x6E, 0x70, 0x75, 0x74, 0x29, 0x3B, 0x74, 0x65,
	0x6D, 0x70, 0x49, 0x6E, 0x70, 0x75, 0x74, 0x2E, 0x73, 0x65, 0x6C, 0x65,
	0x63, 0x74, 0x28, 0x29, 0x3B, 0x74, 0x65, 0x6D, 0x70, 0x49, 0x6E, 0x70,
	0x75, 0x74, 0x2E, 0x73, 0x65, 0x74, 0x53, 0x65, 0x6C, 0x65, 0x63, 0x74,
	0x69, 0x6F, 0x6E, 0x52, 0x61, 0x6E, 0x67, 0x65, 0x28, 0x30, 0x2C, 0x20,
	0x39, 0x39, 0x39, 0x39, 0x39, 0x29, 0x3B, 0x64, 0x6F, 0x63, 0x75, 0x6D,
	0x65, 0x6E, 0x74, 0x2E, 0x65, 0x78, 0x65, 0x63, 0x43, 0x6F, 0x6D, 0x6D,
	0x61, 0x6E, 0x64, 0x28, 0x22, 0x63, 0x6F, 0x70, 0x79, 0x22, 0x29, 0x3B,
	0x64, 0x6F, 0x63, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x2E, 0x62, 0x6F, 0x64,
	0x79, 0x2E, 0x72, 0x65, 0x6D, 0x6F, 0x76, 0x65, 0x43, 0x68, 0x69, 0x6C,
	0x64, 0x28, 0x74, 0x65, 0x6D, 0x70, 0x49, 0x6E, 0x70, 0x75, 0x74, 0x29,
	0x3B, 0x7D, 0x63, 0x6F, 0x6E, 0x73, 0x74, 0x20, 0x63, 0x6F, 0x70, 0x79,
	0x42, 0x75, 0x74, 0x74, 0x6F, 0x6E, 0x3D, 0x64, 0x6F, 0x63, 0x75, 0x6D,
	0x65, 0x6E, 0x74, 0x2E, 0x67, 0x65, 0x74, 0x45, 0x6C, 0x65, 0x6D, 0x65,
	0x6E, 0x74, 0x42, 0x79, 0x49, 0x64, 0x28, 0x22, 0x63, 0x6F, 0x70, 0x79,
	0x42, 0x75, 0x74, 0x74, 0x6F, 0x6E, 0x22, 0x29, 0x3B, 0x63, 0x6F, 0x70,
	0x79, 0x42, 0x75, 0x74, 0x74, 0x6F, 0x6E, 0x2E, 0x61, 0x64, 0x64, 0x45,
	0x76, 0x65, 0x6E, 0x74, 0x4C, 0x69, 0x73, 0x74, 0x65, 0x6E, 0x65, 0x72,
	0x28, 0x22, 0x63, 0x6C, 0x69, 0x63, 0x6B, 0x22, 0x2C, 0x66, 0x75, 0x6E,
	0x63, 0x74, 0x69, 0x6F, 0x6E, 0x28, 0x29, 0x7B, 0x63, 0x6F, 0x6E, 0x73,
	0x74, 0x20, 0x42, 0x61, 0x72, 0x43, 0x6F, 0x6C, 0x6F, 0x72, 0x45, 0x6C,
	0x65, 0x6D, 0x65, 0x6E, 0x74, 0x3D, 0x64, 0x6F, 0x63, 0x75, 0x6D, 0x65,
	0x6E, 0x74, 0x2E, 0x67, 0x65, 0x74, 0x45, 0x6C, 0x65, 0x6D, 0x65, 0x6E,
	0x74, 0x42, 0x79, 0x49, 0x64, 0x28, 0x22, 0x42, 0x61, 0x72, 0x43, 0x6F,
	0x6C, 0x6F, 0x72, 0x22, 0x29, 0x3B, 0x63, 0x6F, 0x6E, 0x73, 0x74, 0x20,
	0x50, 0x72, 0x6F, 0x6D, 0x70, 0x74, 0x43, 0x6F, 0x70, 0x79, 0x45, 0x6C,
	0x65, 0x6D, 0x65, 0x6E, 0x74, 0x3D, 0x64, 0x6F, 0x63, 0x75, 0x6D, 0x65,
	0x6E, 0x74, 0x2E, 0x67, 0x65, 0x74, 0x45, 0x6C, 0x65, 0x6D, 0x65, 0x6E,
	0x74, 0x42, 0x79, 0x49, 0x64, 0x28, 0x22, 0x50, 0x72, 0x6F, 0x6D, 0x70,
	0x74, 0x43, 0x6F, 0x70, 0x79, 0x22, 0x29, 0x3B, 0x63, 0x6F, 0x6E, 0x73,
	0x74, 0x20, 0x50, 0x72, 0x6F, 0x6D, 0x70, 0x74, 0x43, 0x6F, 0x70, 0x79,
	0x3D, 0x50, 0x72, 0x6F, 0x6D, 0x70, 0x74, 0x43, 0x6F, 0x70, 0x79, 0x45,
	0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x2E, 0x69, 0x6E, 0x6E, 0x65, 0x72,
	0x54, 0x65, 0x78, 0x74, 0x3B, 0x63, 0x6F, 0x70, 0x79, 0x54, 0x6F, 0x43,
	0x6C, 0x69, 0x70, 0x62, 0x6F, 0x61, 0x72, 0x64, 0x28, 0x50, 0x72, 0x6F,
	0x6D, 0x70, 0x74, 0x43, 0x6F, 0x70, 0x79, 0x29, 0x3B, 0x42, 0x61, 0x72,
	0x43, 0x6F, 0x6C, 0x6F, 0x72, 0x45, 0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74,
	0x2E, 0x73, 0x74, 0x79, 0x6C, 0x65, 0x2E, 0x62, 0x61, 0x63, 0x6B, 0x67,
	0x72, 0x6F, 0x75, 0x6E, 0x64, 0x3D, 0x22, 0x6C, 0x69, 0x6E, 0x65, 0x61,
	0x72, 0x2D, 0x67, 0x72, 0x61, 0x64, 0x69, 0x65, 0x6E, 0x74, 0x28, 0x74,
	0x6F, 0x20, 0x72, 0x69, 0x67, 0x68, 0x74, 0x2C, 0x20, 0x23, 0x31, 0x36,
	0x32, 0x32, 0x32, 0x46, 0x20, 0x30, 0x25, 0x2C, 0x23, 0x31, 0x36, 0x32,
	0x32, 0x32, 0x38, 0x20, 0x31, 0x30, 0x30, 0x25, 0x29, 0x22, 0x3B, 0x42,
	0x61, 0x72, 0x43, 0x6F, 0x6C, 0x6F, 0x72, 0x45, 0x6C, 0x65, 0x6D, 0x65,
	0x6E, 0x74, 0x2E, 0x73, 0x74, 0x79, 0x6C, 0x65, 0x2E, 0x63, 0x6F, 0x6C,
	0x6F, 0x72, 0x3D, 0x22, 0x44, 0x61, 0x72, 0x6B, 0x4F, 0x72, 0x61, 0x6E,
	0x67, 0x65, 0x22, 0x3B, 0x50, 0x72, 0x6F, 0x6D, 0x70, 0x74, 0x43, 0x6F,
	0x70, 0x79, 0x45, 0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x2E, 0x73, 0x74,
	0x79, 0x6C, 0x65, 0x2E, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3D, 0x22, 0x64,
	0x61, 0x72, 0x6B, 0x63, 0x79, 0x61, 0x6E, 0x22, 0x3B, 0x63, 0x6F, 0x70,
	0x79, 0x42, 0x75, 0x74, 0x74, 0x6F, 0x6E, 0x2E, 0x70, 0x61, 0x72, 0x65,
	0x6E, 0x74, 0x4E, 0x6F, 0x64, 0x65, 0x2E, 0x72, 0x65, 0x6D, 0x6F, 0x76,
	0x65, 0x43, 0x68, 0x69, 0x6C, 0x64, 0x28, 0x63, 0x6F, 0x70, 0x79, 0x42,
	0x75, 0x74, 0x74, 0x6F, 0x6E, 0x29, 0x3B, 0x7D, 0x29, 0x3B, 0x3C, 0x2F,
	0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x3E, 0x3C, 0x2F, 0x62, 0x6F, 0x64,
	0x79, 0x3E, 0x3C, 0x2F, 0x68, 0x74, 0x6D, 0x6C, 0x3E, 0x3C, 0x68, 0x72,
	0x3E, 0x3C, 0x66, 0x6F, 0x6F, 0x74, 0x65, 0x72, 0x3E, 0x3C, 0x61, 0x20,
	0x68, 0x72, 0x65, 0x66, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3A,
	0x2F, 0x2F, 0x67, 0x69, 0x74, 0x68, 0x75, 0x62, 0x2E, 0x63, 0x6F, 0x6D,
	0x2F, 0x43, 0x6C, 0x65, 0x61, 0x73, 0x62, 0x79, 0x43, 0x6F, 0x64, 0x65,
	0x2F, 0x69, 0x6D, 0x67, 0x70, 0x72, 0x6D, 0x74, 0x22, 0x3E, 0x69, 0x6D,
	0x67, 0x70, 0x72, 0x6D, 0x74, 0x3C, 0x2F, 0x61, 0x3E, 0x3C, 0x2F, 0x66,
	0x6F, 0x6F, 0x74, 0x65, 0x72, 0x3E, 0x3C, 0x73, 0x63, 0x72, 0x69, 0x70,
	0x74, 0x20, 0x74, 0x79, 0x70, 0x65, 0x3D, 0x22, 0x74, 0x65, 0x78, 0x74,
	0x2F, 0x6A, 0x61, 0x76, 0x61, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x22,
	0x3E, 0x3C, 0x2F, 0x62, 0x6F, 0x64, 0x79, 0x3E, 0x3C, 0x2F, 0x68, 0x74,
	0x6D, 0x6C, 0x3E
	},

		// Read and store user JPG image file into vector "ImageVec".
		ImageVec((std::istreambuf_iterator<char>(readImage)), std::istreambuf_iterator<char>());

	const std::string
		JPG_SIG = "\xFF\xD8\xFF",	// JPG image signature. 
		JPG_CHECK{ ImageVec.begin(), ImageVec.begin() + JPG_SIG.length() };	// Get image header from vector. 

	// Make sure we have a valid JPG image file.
	if (JPG_CHECK != JPG_SIG) {
		std::cerr << "\nImage Error: File does not appear to be a valid JPG image.\n\n";
		std::exit(EXIT_FAILURE);
	};

	const std::string
		EXIF_SIG = "Exif\x00\x00II",
		EXIF_END_SIG = "xpacket end",
		ICC_PROFILE_SIG = "ICC_PROFILE";

	// An embedded jpg thumbnail will cause problems with this program. So search and remove blocks that may contain a jpg thumbnail.

	// Check for a ICC_PROFILE and delete all content before the beginning of Profile. This removes any embedded jpg thumbnail. Profile will get deleted later.
	const size_t ICC_PROFILE_POS = search(ImageVec.begin(), ImageVec.end(), ICC_PROFILE_SIG.begin(), ICC_PROFILE_SIG.end()) - ImageVec.begin();
	if (ImageVec.size() > ICC_PROFILE_POS) {
		ImageVec.erase(ImageVec.begin(), ImageVec.begin() + ICC_PROFILE_POS);
	}

	// If no Profile found, search for Exif block (look for end signature "xpacket end") and remove the block.
	const size_t EXIF_END_POS = search(ImageVec.begin(), ImageVec.end(), EXIF_END_SIG.begin(), EXIF_END_SIG.end()) - ImageVec.begin();
	if (ImageVec.size() > EXIF_END_POS) {
		ImageVec.erase(ImageVec.begin(), ImageVec.begin() + (EXIF_END_POS + 17));
	}

	// Remove Exif block that has no "xpacket end" signature.
	const size_t EXIF_START_POS = search(ImageVec.begin(), ImageVec.end(), EXIF_SIG.begin(), EXIF_SIG.end()) - ImageVec.begin();
	if (ImageVec.size() > EXIF_START_POS) {
		// get size of Exif block
		const size_t EXIF_BLOCK_SIZE = ImageVec[EXIF_START_POS - 2] << 8 | ImageVec[EXIF_START_POS - 1];
		ImageVec.erase(ImageVec.begin(), ImageVec.begin() + EXIF_BLOCK_SIZE - 2);
	}

	// ^ Any jpg embedded thumbnail should have now been removed...
	
	// Signature for Define Quantization Table(s) 
	const auto DQT_SIG = { 0xFF, 0xDB };
	
	// Find location in vector "ImageVec" of first DQT index location of the image file.
	const size_t DQT_POS = search(ImageVec.begin(), ImageVec.end(), DQT_SIG.begin(), DQT_SIG.end()) - ImageVec.begin();

	// Erase the first n bytes of the JPG header before the DQT position. 
	// Later, we will replace the erased header with the contents of vector "ProfileVec".
	ImageVec.erase(ImageVec.begin(), ImageVec.begin() + DQT_POS);
	
	std::wstring outName = L"imgprmt_pic.jpg";

	std::string
		prompt,
		link;
	
	std::cout << "\n\nEnter a URL (Image source, Social media page, etc.)\n\n: ";
	std::getline(std::cin, link);

	fflush(stdin); fflush(stdout);
	std::ignore = _setmode(_fileno(stdin), 0x20000);  // Set standard input for Windows to "_O_U16TEXT" so our wide variables contain correct wide characters.
	std::ignore = _setmode(_fileno(stdout), 0x20000); // Set standard output for Windows to "_O_U16TEXT" so the console displays correct wide characters.
	
	std::wcout << "\nType or Paste in Your Image Prompt / Description\n\n: ";

	const size_t wcinBufferSize = 8000;  	// The default string length in Windows console for "wcin" when "_O_U16TEXT" is set, 
						// seems to be just over 2000 characters. So we increase it here to 8000 to provide a more flexible size.

	wchar_t wcinBuffer[wcinBufferSize] = { 0 };
	std::wcin.rdbuf()->pubsetbuf(wcinBuffer, wcinBufferSize); // Set the new buffer size limit for "wcin", as above.

	std::wcin.getline(wcinBuffer, wcinBufferSize);
	std::wstring w_prompt(wcinBuffer);
	
	// Search the wide string for certain characters and replace them with html entity codes.
	replaceSpecialChars(w_prompt);

	// We have now replaced the wide characters with the html entity codes, so no wide characters should exist in 
	// the prompt/description string. Convert the wide string back to a standard string so that we can insert it
	// into the vector "ProfileVec", in-turn, "ProfileVec" will be inserted into the vector "ImageVec", then written out to file.

	std::transform(w_prompt.begin(), w_prompt.end(), std::back_inserter(prompt), [](wchar_t c) {
		return (char)c;
		});

	const int
		MAX_PROFILE_SIZE = 10000,		// For Twitter compatibility. Twitter only allows one ICC Profile, with a max size of 10KB. 
		PROFILE_MAIN_DIFF = 0x16,		// Bytes we don't count as part of profile size.
		PROFILE_INTERNAL_DIFF = 0x26,		// Bytes we don't count as part of internal profile size.
		PROMPT_INSERT_INDEX = 0x9A4,		// Insert location within ProfileVec of the HTML page for the users's prompt text.
		LINK_INSERT_INDEX = 0x91E;		// Insert location within ProfileVec of the HTML page for the user's web link (url).

	int
		bits = 0x10,
		profileSizeField = 0x16,		// Start index location for size field of the main image icc profile. (Max two bytes)
		profileInternalSizeField = 0x28;	// Start index location for internal size field of the image icc profile.(Max four bytes, only two used).

	// Insert image prompt/description & url link into their relevant index positions within vector "ProfileVec".
	ProfileVec.insert(ProfileVec.begin() + PROMPT_INSERT_INDEX, prompt.begin(), prompt.end());
	ProfileVec.insert(ProfileVec.begin() + LINK_INSERT_INDEX, link.begin(), link.end());

	if (ProfileVec.size() > MAX_PROFILE_SIZE) {
		std::wcerr << "\nFile Size Error: Data content size exceeds the maximum limit of 10KB.\n\n";
		std::exit(EXIT_FAILURE);
	}

	// Update main profile size 
	update->Value(ProfileVec, profileSizeField, ProfileVec.size() - PROFILE_MAIN_DIFF, bits);

	// Update internal profile size
	update->Value(ProfileVec, profileInternalSizeField, ProfileVec.size() - PROFILE_INTERNAL_DIFF, bits);

	// Insert contents of vector ProfileVec into vector ImageVec, combining the profile, html and prompt/link text with JPG image file.
	ImageVec.insert(ImageVec.begin(), ProfileVec.begin(), ProfileVec.end());

	// Write the complete JPG / HTML polyglot out to file.
	std::ofstream writeFile(outName, std::ios::binary);

	if (!writeFile) {
		std::cerr << "\nWrite Error: Unable to write to file.\n\n";
		std::exit(EXIT_FAILURE);
	}

	writeFile.write((char*)&ImageVec[0], ImageVec.size());
	std::wcout << L"\nCreated output file: \"" + outName + L" " << ImageVec.size() << " " << "Bytes\"\n\n";
}

void displayInfo() {
	std::cout << R"(
		
Imgprmt v1.1 (Windows). Created by Nicholas Cleasby (@CleasbyCode) 25/05/2023.
		
This program enables you to insert a prompt/description for your AI images within a JPG image file.
		
The program will create a JPG / HTML polyglot file.
		
Your prompt text is saved within a basic HTML page embedded within the image, that you can view by renaming
the ".jpg" file extension to a ".html" extension.

The image supports Twitter, Mastodon, Flickr, Imgur & *Reddit (*Desktop only. Does not work with Reddit mobile app). 

This means the image can be shared on these platforms & it will retain the embedded HTML page with your prompt text.

)";
}
