#include "programArgs.h"
#include "information.h"

#include <stdexcept>
#include <cstdlib>

ProgramArgs ProgramArgs::parse(int argc, char** argv) {
	ProgramArgs args;
	if (argc == 2 && std::string(argv[1]) == "--info") {
		displayInfo();
        std::exit(0);
	}

	if (argc < 2 || argc > 3) {
        throw std::runtime_error("Usage: imgprmt [-b|-x] <jpg-image>\n\t\bimgprmt --info");
    }

    int arg_index = 1;

    if (argc == 3) {
		if (std::string(argv[arg_index]) != "-b" && std::string(argv[arg_index]) != "-x") {
        	throw std::runtime_error("Input Error: Invalid arguments. Expecting \"-b\" or \"-x\" as the only optional arguments.");
        }
		if (std::string(argv[arg_index]) == "-b") {
        	args.platform = ArgOption::Bluesky;
		} else {
			args.platform = ArgOption::Twitter;
		}
        	arg_index = 2;
    }
    args.cover_image = argv[arg_index];
    return args;
}
