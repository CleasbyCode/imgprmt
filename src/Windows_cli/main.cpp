//	imgprmt v1.5 (Windows CLI Edition). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023


enum class ArgOption {
	Default,
	BlueSky
};

#include "imgprmt.h"

int main(int argc, char** argv) {
	if (argc == 2 && std::string(argv[1]) == "--info") {
		displayInfo();
		return 0;
	}
	if (argc < 2 || argc > 3) {
		std::cout << "\nUsage: imgprmt [-b] <jpg-image>\n\t\bimgprmt --info\n\n";
		return 1;
	}

	ArgOption platformOption = ArgOption::Default;
	uint8_t argIndex = 1;

	if (argc == 3) {
		if (std::string(argv[1]) != "-b") {
			std::cerr << "\nInput Error: Invalid arguments. Expecting \"-b\" as the only optional argument.\n\n";
			return 1;
		}
		platformOption = ArgOption::BlueSky;
		argIndex = 2;
	}

	const std::string IMAGE_FILENAME = argv[argIndex];

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
	imgPrmt(IMAGE_FILENAME, platformOption);
}
