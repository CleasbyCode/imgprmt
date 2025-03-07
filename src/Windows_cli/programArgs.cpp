enum class ArgOption {
	Default,
	Bluesky
};

struct ProgramArgs {
	ArgOption platform = ArgOption::Default;
    	std::string image_file;

    	static ProgramArgs parse(int argc, char** argv);
};

ProgramArgs ProgramArgs::parse(int argc, char** argv) {
	ProgramArgs args;
	if (argc == 2 && std::string(argv[1]) == "--info") {
		displayInfo();
        	std::exit(0);
	}

	if (argc < 2 || argc > 3) {
        	throw std::runtime_error("Usage: imgprmt [-b] <jpg-image>\n\t\bimgprmt --info");
    	}

    	uint8_t arg_index = 1;

    	if (argc == 3) {
		if (std::string(argv[arg_index]) != "-b") {
            		throw std::runtime_error("Input Error: Invalid arguments. Expecting \"-b\" as the only optional argument.");
        	}
        	args.platform = ArgOption::Bluesky;
        	++arg_index;
    	}

    	args.image_file = argv[arg_index];
    	return args;
}
