#pragma once

#include <string>

enum class ArgOption {
	Default,
	Bluesky,
	Twitter
};

struct ProgramArgs {
	ArgOption platform = ArgOption::Default;
    	std::string cover_image;

    	static ProgramArgs parse(int argc, char** argv);
};
