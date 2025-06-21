#pragma once

#include <string>

enum class ArgOption {
	Default,
	Bluesky,
	Twitter
};

struct ProgramArgs {
	ArgOption platform = ArgOption::Default;
    	std::string image_file;

    	static ProgramArgs parse(int argc, char** argv);
};
