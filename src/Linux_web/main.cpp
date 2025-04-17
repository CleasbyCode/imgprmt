//	imgprmt v1.5 (Linux edition for imgprmt website ). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023
//	Not for standard use. ONLY used by the Web App: https://cleasbycode.co.uk/imgprmt/app/
//	If you want the imgprmt CLI tool, then download/compile the imgprmt Windows edition from the github page.

enum class ArgOption {
	Default,
	BlueSky
};

#include "imgprmt.h"

int main(int argc, char** argv) {

	ArgOption platformOption = ArgOption::Default;
	uint8_t argIndex = 1;

	if (argc == 5) {
		platformOption = ArgOption::BlueSky;
		argIndex = 2;
	}

	const std::string IMAGE_FILENAME = argv[argIndex];
	const std::string PROMPT_FILENAME = argv[++argIndex];
	const std::string URL_FILENAME = argv[++argIndex];

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

	const std::filesystem::path PROMPT_PATH(PROMPT_FILENAME);
	const std::string PROMPT_EXTENSION = PROMPT_PATH.extension().string();

	const std::filesystem::path URL_PATH(URL_FILENAME);
	const std::string URL_EXTENSION = URL_PATH.extension().string();

	if (!std::filesystem::exists(IMAGE_FILENAME)) {
		std::cerr << "\nImage File Error: File not found. Check the filename and try again.\n\n";
		return 1;
	}

	if (!std::filesystem::exists(PROMPT_FILENAME) || !std::filesystem::exists(URL_FILENAME)) {
		std::cerr << "\nFile Error: File not found. One or more of the crucial text files is missing. Check the filename and try again.\n\n";
		return 1;
	}

	imgPrmt(IMAGE_FILENAME, PROMPT_FILENAME, URL_FILENAME, platformOption);
}
