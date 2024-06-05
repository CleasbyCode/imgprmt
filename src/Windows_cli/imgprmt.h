#pragma once

#include <algorithm>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <regex>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <io.h>

#include "searchFunc.cpp"
#include "replaceChars.cpp"
#include "profileData.cpp"
#include "valueUpdater.cpp"
#include "information.cpp"
#include "imgprmt.cpp"

template <uint_fast8_t N>
uint_fast32_t searchFunc(std::vector<uint_fast8_t>&, uint_fast32_t, uint_fast8_t, const uint_fast8_t (&)[N]);

void
	startImgprmt(const std::string&),
	replaceProblemChars(std::wstring& str),
	valueUpdater(std::vector<uint_fast8_t>&, uint_fast32_t, const uint_fast32_t, uint_fast8_t),
	displayInfo();