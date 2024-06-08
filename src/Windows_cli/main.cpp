//	imgprmt v1.3 (Windows CLI Edition). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023

#include "imgprmt.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cout << "\nUsage: imgprmt <jpg-image>\n\t\bimgprmt --info\n\n";
	} else if (std::string(argv[1]) == "--info") {
		displayInfo();
	} else {
		const std::string IMAGE_FILENAME = std::string(argv[1]);

		const std::regex REG_EXP("(\\.[a-zA-Z_0-9\\.\\\\\\s\\-\\/]+)?[a-zA-Z_0-9\\.\\\\\\s\\-\\/]+?(\\.[a-zA-Z0-9]+)?");

		std::string file_extension = IMAGE_FILENAME.length() > 3 ? IMAGE_FILENAME.substr(IMAGE_FILENAME.length() - 4) : IMAGE_FILENAME;

		file_extension = file_extension == "jpeg" || file_extension == "jiff" ? ".jpg" : file_extension;

		if (file_extension == ".jpg" && regex_match(IMAGE_FILENAME, REG_EXP) && std::filesystem::exists(IMAGE_FILENAME)) {
			startImgprmt(IMAGE_FILENAME);
		} else {
			std::cerr << (file_extension != ".jpg"
				? "\nFile Type Error: Invalid file extension found. Expecting only \"jpg\""
				: !regex_match(IMAGE_FILENAME, REG_EXP)
					? "\nInvalid Input Error: Characters not supported by this program found within filename arguments"
					: "\nImage File Error: File not found. Check the filename and try again")
			<< ".\n\n";

			std::exit(EXIT_FAILURE);
		}
	}
}
