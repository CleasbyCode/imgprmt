//	imgprmt v1.4 (Windows CLI Edition). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023
//	Open imgprmt.sln file in Visual Studio, then from the Build menu, select build solution.

#include "imgprmt.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "\nUsage: imgprmt <jpg-image>\n\t\bimgprmt --info\n\n";
		return 1;
	}
	if (std::string(argv[1]) == "--info") {
		displayInfo();
		return 0;
	}

	const std::string IMAGE_FILENAME = std::string(argv[1]);

	constexpr const char* REG_EXP = ("(\\.[a-zA-Z_0-9\\.\\\\\\s\\-\\/]+)?[a-zA-Z_0-9\\.\\\\\\s\\-\\/]+?(\\.[a-zA-Z0-9]+)?");
	const std::regex regex_pattern(REG_EXP);

	if (!std::regex_match(IMAGE_FILENAME, regex_pattern)) {
		std::cerr << "\nInvalid Input Error: Characters not supported by this program found within filename arguments.\n\n";
		return 1;
	}

	const std::filesystem::path IMAGE_PATH(IMAGE_FILENAME);
	const std::string IMAGE_EXTENSION = IMAGE_PATH.extension().string();

	if (IMAGE_EXTENSION != ".jpg" && IMAGE_EXTENSION != ".jpeg" && IMAGE_EXTENSION != ".jfif") {
		std::cerr << "\nFile Type Error: Invalid file extension. Expecting only \"jpg, jpeg or jfif\" image extensions.\n\n";
		return 1;
	}

	if (!std::filesystem::exists(IMAGE_FILENAME)) {
		std::cerr << "\nImage File Error: File not found. Check the filename and try again.\n\n";
		return 1;
	}
	imgPrmt(IMAGE_FILENAME);
}
