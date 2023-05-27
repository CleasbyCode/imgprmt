//	imgprmt for Twitter & Reddit & Imgur. Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class ValueUpdater {
public:
	void Value(std::vector<unsigned char>& vect, int valueInsertIndex, const size_t VALUE, int bits) {
			while (bits) {
				vect[valueInsertIndex++] = (VALUE >> (bits -= 8)) & 0xff;
			}
		}
} *update;

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
		std::cout << "\nUsage:\t\bimgprmt <jpg-image>\n\t\bimgprmt --info\n\n";
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
	// currently without the users image prompt/link, which is inserted later.
	std::vector<unsigned char>ProfileVec = {
	0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0xFF, 0xE2, 0x07, 0x71,
	0x49, 0x43, 0x43, 0x5F, 0x50, 0x52, 0x4F, 0x46, 0x49, 0x4C, 0x45, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x07, 0x61, 0x3C, 0x21, 0x2D, 0x2D, 0x04, 0x30,
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
	0x00, 0x09, 0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00,
	0x00, 0x24, 0x72, 0x58, 0x59, 0x5A, 0x00, 0x00, 0x01, 0x14, 0x00, 0x00,
	0x00, 0x14, 0x67, 0x58, 0x59, 0x5A, 0x00, 0x00, 0x01, 0x28, 0x00, 0x00,
	0x00, 0x14, 0x62, 0x58, 0x59, 0x5A, 0x00, 0x00, 0x01, 0x3C, 0x00, 0x00,
	0x00, 0x14, 0x77, 0x74, 0x70, 0x74, 0x00, 0x00, 0x01, 0x50, 0x00, 0x00,
	0x00, 0x14, 0x72, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x64, 0x00, 0x00,
	0x00, 0x28, 0x67, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x64, 0x00, 0x00,
	0x00, 0x28, 0x62, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0x64, 0x00, 0x00,
	0x00, 0x28, 0x63, 0x70, 0x72, 0x74, 0x00, 0x00, 0x01, 0x8C, 0x00, 0x00,
	0x00, 0x00, 0x6D, 0x6C, 0x75, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
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
	0x50, 0x45, 0x20, 0x68, 0x74, 0x6D, 0x6C, 0x3E, 0x3C, 0x6D, 0x65, 0x74,
	0x61, 0x20, 0x6E, 0x61, 0x6D, 0x65, 0x3D, 0x22, 0x76, 0x69, 0x65, 0x77,
	0x70, 0x6F, 0x72, 0x74, 0x22, 0x20, 0x63, 0x6F, 0x6E, 0x74, 0x65, 0x6E,
	0x74, 0x3D, 0x22, 0x77, 0x69, 0x64, 0x74, 0x68, 0x3D, 0x64, 0x65, 0x76,
	0x69, 0x63, 0x65, 0x2D, 0x77, 0x69, 0x64, 0x74, 0x68, 0x2C, 0x20, 0x69,
	0x6E, 0x69, 0x74, 0x69, 0x61, 0x6C, 0x2D, 0x73, 0x63, 0x61, 0x6C, 0x65,
	0x3D, 0x31, 0x22, 0x3E, 0x3C, 0x68, 0x74, 0x6D, 0x6C, 0x3E, 0x3C, 0x6C,
	0x69, 0x6E, 0x6B, 0x20, 0x68, 0x72, 0x65, 0x66, 0x3D, 0x22, 0x68, 0x74,
	0x74, 0x70, 0x73, 0x3A, 0x2F, 0x2F, 0x66, 0x6F, 0x6E, 0x74, 0x73, 0x2E,
	0x67, 0x6F, 0x6F, 0x67, 0x6C, 0x65, 0x61, 0x70, 0x69, 0x73, 0x2E, 0x63,
	0x6F, 0x6D, 0x2F, 0x63, 0x73, 0x73, 0x3F, 0x66, 0x61, 0x6D, 0x69, 0x6C,
	0x79, 0x3A, 0x20, 0x43, 0x61, 0x6C, 0x69, 0x62, 0x72, 0x69, 0x22, 0x20,
	0x72, 0x65, 0x6C, 0x3D, 0x22, 0x73, 0x74, 0x79, 0x6C, 0x65, 0x73, 0x68,
	0x65, 0x65, 0x74, 0x22, 0x3E, 0x3C, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74,
	0x20, 0x74, 0x79, 0x70, 0x65, 0x3D, 0x22, 0x74, 0x65, 0x78, 0x74, 0x2F,
	0x6A, 0x61, 0x76, 0x61, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x22, 0x3E,
	0x64, 0x6F, 0x63, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x2E, 0x62, 0x6F, 0x64,
	0x79, 0x2E, 0x69, 0x6E, 0x6E, 0x65, 0x72, 0x48, 0x54, 0x4D, 0x4C, 0x20,
	0x3D, 0x20, 0x27, 0x27, 0x3B, 0x76, 0x61, 0x72, 0x20, 0x70, 0x61, 0x74,
	0x68, 0x20, 0x3D, 0x20, 0x77, 0x69, 0x6E, 0x64, 0x6F, 0x77, 0x2E, 0x6C,
	0x6F, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x2E, 0x70, 0x61, 0x74, 0x68,
	0x6E, 0x61, 0x6D, 0x65, 0x3B, 0x76, 0x61, 0x72, 0x20, 0x66, 0x6E, 0x61,
	0x6D, 0x65, 0x20, 0x3D, 0x20, 0x70, 0x61, 0x74, 0x68, 0x2E, 0x73, 0x70,
	0x6C, 0x69, 0x74, 0x28, 0x22, 0x2F, 0x22, 0x29, 0x2E, 0x70, 0x6F, 0x70,
	0x28, 0x29, 0x3B, 0x64, 0x6F, 0x63, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x2E,
	0x62, 0x6F, 0x64, 0x79, 0x2E, 0x69, 0x6E, 0x6E, 0x65, 0x72, 0x48, 0x54,
	0x4D, 0x4C, 0x20, 0x3D, 0x27, 0x3C, 0x62, 0x6F, 0x64, 0x79, 0x3E, 0x3C,
	0x73, 0x74, 0x79, 0x6C, 0x65, 0x3E, 0x62, 0x6F, 0x64, 0x79, 0x20, 0x7B,
	0x62, 0x61, 0x63, 0x6B, 0x67, 0x72, 0x6F, 0x75, 0x6E, 0x64, 0x2D, 0x63,
	0x6F, 0x6C, 0x6F, 0x72, 0x3A, 0x44, 0x61, 0x72, 0x6B, 0x42, 0x6C, 0x75,
	0x65, 0x3B, 0x7D, 0x62, 0x72, 0x20, 0x7B, 0x6C, 0x69, 0x6E, 0x65, 0x2D,
	0x68, 0x65, 0x69, 0x67, 0x68, 0x74, 0x3A, 0x20, 0x30, 0x70, 0x78, 0x3B,
	0x7D, 0x69, 0x6D, 0x67, 0x20, 0x7B, 0x62, 0x6F, 0x72, 0x64, 0x65, 0x72,
	0x3A, 0x20, 0x32, 0x70, 0x78, 0x20, 0x73, 0x6F, 0x6C, 0x69, 0x64, 0x20,
	0x23, 0x37, 0x30, 0x39, 0x3B, 0x62, 0x6F, 0x72, 0x64, 0x65, 0x72, 0x2D,
	0x72, 0x61, 0x64, 0x69, 0x75, 0x73, 0x3A, 0x20, 0x31, 0x70, 0x78, 0x3B,
	0x70, 0x61, 0x64, 0x64, 0x69, 0x6E, 0x67, 0x3A, 0x20, 0x32, 0x70, 0x78,
	0x3B, 0x77, 0x69, 0x64, 0x74, 0x68, 0x3A, 0x20, 0x32, 0x34, 0x38, 0x70,
	0x78, 0x3B, 0x7D, 0x69, 0x6D, 0x67, 0x3A, 0x68, 0x6F, 0x76, 0x65, 0x72,
	0x20, 0x7B, 0x62, 0x6F, 0x78, 0x2D, 0x73, 0x68, 0x61, 0x64, 0x6F, 0x77,
	0x3A, 0x20, 0x30, 0x20, 0x30, 0x20, 0x34, 0x70, 0x78, 0x20, 0x34, 0x70,
	0x78, 0x20, 0x72, 0x67, 0x62, 0x61, 0x28, 0x32, 0x35, 0x35, 0x2C, 0x32,
	0x35, 0x35, 0x2C, 0x30, 0x29, 0x3B, 0x7D, 0x68, 0x31, 0x20, 0x7B, 0x63,
	0x6F, 0x6C, 0x6F, 0x72, 0x3A, 0x20, 0x6F, 0x72, 0x61, 0x6E, 0x67, 0x65,
	0x3B, 0x74, 0x65, 0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E, 0x3A,
	0x20, 0x63, 0x65, 0x6E, 0x74, 0x65, 0x72, 0x3B, 0x66, 0x6F, 0x6E, 0x74,
	0x2D, 0x73, 0x69, 0x7A, 0x65, 0x3A, 0x20, 0x33, 0x33, 0x70, 0x78, 0x3B,
	0x7D, 0x68, 0x33, 0x20, 0x7B, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3A, 0x20,
	0x23, 0x44, 0x46, 0x44, 0x33, 0x45, 0x33, 0x3B, 0x66, 0x6F, 0x6E, 0x74,
	0x2D, 0x66, 0x61, 0x6D, 0x69, 0x6C, 0x79, 0x3A, 0x20, 0x43, 0x61, 0x6C,
	0x69, 0x62, 0x72, 0x69, 0x3B, 0x66, 0x6F, 0x6E, 0x74, 0x2D, 0x73, 0x69,
	0x7A, 0x65, 0x3A, 0x20, 0x32, 0x32, 0x70, 0x78, 0x3B, 0x20, 0x74, 0x65,
	0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E, 0x3A, 0x20, 0x73, 0x74,
	0x61, 0x72, 0x74, 0x3B, 0x74, 0x65, 0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69,
	0x67, 0x6E, 0x3A, 0x20, 0x65, 0x6E, 0x64, 0x3B, 0x74, 0x65, 0x78, 0x74,
	0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E, 0x3A, 0x20, 0x6C, 0x65, 0x66, 0x74,
	0x3B, 0x74, 0x65, 0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E, 0x3A,
	0x20, 0x72, 0x69, 0x67, 0x68, 0x74, 0x3B, 0x74, 0x65, 0x78, 0x74, 0x2D,
	0x61, 0x6C, 0x69, 0x67, 0x6E, 0x3A, 0x20, 0x63, 0x65, 0x6E, 0x74, 0x65,
	0x72, 0x3B, 0x74, 0x65, 0x78, 0x74, 0x2D, 0x61, 0x6C, 0x69, 0x67, 0x6E,
	0x3A, 0x20, 0x6A, 0x75, 0x73, 0x74, 0x69, 0x66, 0x79, 0x3B, 0x6D, 0x61,
	0x72, 0x67, 0x69, 0x6E, 0x3A, 0x32, 0x30, 0x70, 0x78, 0x20, 0x32, 0x34,
	0x38, 0x70, 0x78, 0x3B, 0x7D, 0x3C, 0x2F, 0x73, 0x74, 0x79, 0x6C, 0x65,
	0x3E, 0x3C, 0x62, 0x72, 0x3E, 0x3C, 0x68, 0x31, 0x3E, 0x49, 0x6D, 0x61,
	0x67, 0x65, 0x20, 0x50, 0x72, 0x6F, 0x6D, 0x70, 0x74, 0x20, 0x2F, 0x20,
	0x44, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x3C,
	0x2F, 0x68, 0x31, 0x3E, 0x3C, 0x63, 0x65, 0x6E, 0x74, 0x65, 0x72, 0x3E,
	0x3C, 0x61, 0x20, 0x74, 0x61, 0x72, 0x67, 0x65, 0x74, 0x3D, 0x22, 0x5F,
	0x62, 0x6C, 0x61, 0x6E, 0x6B, 0x22, 0x20, 0x68, 0x72, 0x65, 0x66, 0x3D,
	0x22, 0x22, 0x3E, 0x3C, 0x69, 0x6D, 0x67, 0x20, 0x73, 0x72, 0x63, 0x3D,
	0x22, 0x27, 0x20, 0x2B, 0x20, 0x66, 0x6E, 0x61, 0x6D, 0x65, 0x20, 0x2B,
	0x20, 0x27, 0x22, 0x20, 0x61, 0x6C, 0x74, 0x3D, 0x22, 0x49, 0x6D, 0x61,
	0x67, 0x65, 0x20, 0x50, 0x72, 0x65, 0x76, 0x69, 0x65, 0x77, 0x22, 0x3E,
	0x3C, 0x2F, 0x61, 0x3E, 0x3C, 0x2F, 0x63, 0x65, 0x6E, 0x74, 0x65, 0x72,
	0x3E, 0x3C, 0x68, 0x33, 0x3E, 0x3C, 0x2F, 0x68, 0x33, 0x3E, 0x27, 0x3C,
	0x2F, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x3E, 0x3C, 0x66, 0x6F, 0x6E,
	0x74, 0x20, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x20, 0x3D, 0x22, 0x23, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x22, 0x3E, 0x3C, 0x68, 0x72, 0x3E, 0x3C,
	0x2F, 0x62, 0x6F, 0x64, 0x79, 0x3E, 0x3C, 0x2F, 0x68, 0x74, 0x6D, 0x6C,
	0x3E, 0x3C, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x20, 0x74, 0x79, 0x70,
	0x65, 0x3D, 0x22, 0x74, 0x65, 0x78, 0x74, 0x2F, 0x6A, 0x61, 0x76, 0x61,
	0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x22, 0x3E
	},

	// Read and store user JPG image file into vector ImageVec.
	ImageVec((std::istreambuf_iterator<char>(readImage)), std::istreambuf_iterator<char>());

	const std::string
		JPG_SIG = "\xFF\xD8\xFF",	// JPG image signature. 
		JPG_CHECK{ ImageVec.begin(), ImageVec.begin() + JPG_SIG.length() };	// Get image header from vector. 

	// Make sure we are dealing with a valid JPG image file.
	if (JPG_CHECK != JPG_SIG) {
		std::cerr << "\nImage Error: File does not appear to be a valid JPG image.\n\n";
		std::exit(EXIT_FAILURE);
	};

	// Signature for Define Quantization Table(s) 
	const auto DQT_SIG = { 0xFF, 0xDB };

	// Find location in vector "ImageVec" of first DQT index location of the image file.
	const size_t DQT_POS = search(ImageVec.begin(), ImageVec.end(), DQT_SIG.begin(), DQT_SIG.end()) - ImageVec.begin();

	// Erase the first n bytes of the JPG header before this DQT position. 
  	// We will later replace the erased header with the contents of vector "ProfileVec".
	ImageVec.erase(ImageVec.begin(), ImageVec.begin() + DQT_POS);

	// A bodge-fix for some special characters... Will probably need to add more of these.
	std::vector<std::string>TagVec {"&#39;", "&#960;", "&#225;", "&#201;", "&#233;", "&#243;", "&#227;"};
	char Tag[6] = {39, -29, -96, -112, -126, -94};
  
	std::string
		outName = "imgprmt_pic.jpg",
		link,
		prompt;
  
	std::cout << "\nType / Paste Your Image Description\n: ";
	std::getline(std::cin, prompt);
  
	std::cout << "\n\nEnter a Link (Image Source, Social Media Page, etc.)\n: ";
	std::getline(std::cin, link);

	// A bodge-fix for some special characters...
	for (size_t i = prompt.length(); i != -1; i--) {
		for (int x = 0; x != 6; x++) {      
			if (prompt[i] == Tag[x]) {
				prompt.erase(prompt.begin() + i);
				prompt.insert(prompt.begin() + i, TagVec[x].begin(), TagVec[x].end());
			}
			if (prompt[i] == 97 && prompt[i - 1] == 'S' && prompt[i + 1] == 'o' && prompt[i + 3] == 'P') {
				prompt.erase(prompt.begin() + i);
				prompt.insert(prompt.begin() + i, TagVec[6].begin(), TagVec[6].end());
			}
		}
	}

	const int
		PROFILE_MAIN_DIFF = 0x16,	// Bytes we don't count as part of profile size.
		PROFILE_INTERNAL_DIFF = 0x26,	// Bytes we don't count as part of internal profile size.
		PROMPT_INSERT_INDEX = 0x539,	// Insert location within ProfileVec of the HTML page for the users's prompt text.
		LINK_INSERT_INDEX = 0x4f9;	// Insert location within ProfileVec of the HTML page for the user's web link.

	int
		bits = 0x10,
		profileSizeField = 0x16,		// Start index location for size field of the main image profile.
		profileInternalSizeField = 0x28;	// Start index location for internal size field of the image profile.

	// Insert image description & image link into their relevant index positions within vector ProfileVec.
	ProfileVec.insert(ProfileVec.begin() + PROMPT_INSERT_INDEX, prompt.begin(), prompt.end());
	ProfileVec.insert(ProfileVec.begin() + LINK_INSERT_INDEX, link.begin(), link.end());

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
	std::cout << "\nCreated output file: \"" + outName + " " << ImageVec.size() << " " << "Bytes\"\n\n"; 
	}

	void displayInfo() {
		std::cout << R"(
		
	Imgprmt (v1.0) for Twitter, Reddit & Imgur. Created by Nicholas Cleasby (@CleasbyCode) 25/05/2023.
		
	This program enables you to embed a prompt/description for your AI images within a JPG image file.
		
	JPG / HTML Polyglot File.
		
	The prompt/description is saved within a basic HTML page that you can view just by renaming
	the .jpg file extension to .html.

	The image supports Twitter, Reddit & Imgur. 

	This means you can share your image on the above platforms and it will retain the embedded image description.

	This program works on Linux and Windows.
		)";
	}
