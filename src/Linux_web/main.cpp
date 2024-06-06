//	imgprmt v1.3 (Linux edition for imgprmt website ). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023
//	To be used only with https://cleasbycode.co.uk/imgprmt/app/
//	If you want a imgprmt CLI tool, then use the Windows edition from the github page.

#include "imgprmt.h"

int main(int argc, char** argv) {

	if (argc == 2) {

		const std::string IMAGE_FILENAME = std::string(argv[1]);
		
		std::string file_extension = IMAGE_FILENAME.length() > 3 ? IMAGE_FILENAME.substr(IMAGE_FILENAME.length() - 4) : IMAGE_FILENAME;

		file_extension = file_extension == "jpeg" || file_extension == "jiff" ? ".jpg" : file_extension;

		if (file_extension == ".jpg" && std::filesystem::exists(IMAGE_FILENAME)) {
			startImgprmt(IMAGE_FILENAME);
		} else {
			std::cerr << (file_extension != ".jpg"
				? "\nFile Type Error: Invalid file extension found. Expecting only \"jpg\""
				: "\nImage File Error: File not found. Check the filename and try again")
			<< ".\n\n";

			std::exit(EXIT_FAILURE);
		}
	}
}
