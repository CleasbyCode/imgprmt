#pragma once

#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "searchFunc.cpp"
#include "profileData.cpp"
#include "valueUpdater.cpp"
#include "imgprmt.cpp"

template <uint_fast8_t N>
uint_fast32_t searchFunc(std::vector<uint_fast8_t>&, uint_fast32_t, uint_fast8_t, const uint_fast8_t (&)[N]);

void
	startImgprmt(const std::string&),
	valueUpdater(std::vector<uint_fast8_t>&, uint_fast32_t, const uint_fast32_t, uint_fast8_t);
