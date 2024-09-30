//	imgprmt v1.4 (Linux edition for imgprmt website ). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023
//	To be used ONLY with Web App https://cleasbycode.co.uk/imgprmt/app/
//	If you want the imgprmt CLI tool, then download/compile the imgprmt Windows edition from the github page.

#include "imgprmt.h"

int main(int argc, char** argv) {
	if (argc == 2) {
		const std::string IMAGE_FILENAME = std::string(argv[1]);
		
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
}
