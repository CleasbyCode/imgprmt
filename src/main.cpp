//  imgprmt v1.7 (CLI Edition). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023

//  Downloads/imgprmt-main/src$ sudo apt-get install libturbojpeg-dev
//  Downloads/imgprmt-main/src$ chmod +x compile_imgprmt.sh
//  Downloads/imgprmt-main/src$ ./compile_imgprmt.sh
//  Downloads/imgprmt-main/src$ Compilation successful. Executable 'imgprmt' created.
//  Downloads/imgprmt-main/src$ sudo cp imgprmt /usr/bin

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
