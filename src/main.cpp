//	imgprmt v1.6 (CLI Edition). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023

#include "fileChecks.h"
#include "imgPrmt.h"

#include <iostream>
#include <stdexcept>

int main(int argc, char** argv) {
    try {
        ProgramArgs args = ProgramArgs::parse(argc, argv);
        if (!hasValidFilename(args.image_file)) {
            throw std::runtime_error("Invalid Input Error: Unsupported characters in filename arguments.");
        }
        validateFiles(args.image_file);

        imgPrmt(args.image_file, args.platform);
    }
    catch (const std::runtime_error& e) {
        std::cerr << "\n" << e.what() << "\n\n";
        return 1;
    }
}
