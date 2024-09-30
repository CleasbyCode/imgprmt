#pragma once

#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <regex>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "searchFunc.cpp"
#include "eraseSegments.cpp"
#include "profileData.cpp"
#include "valueUpdater.cpp"
#include "imgprmt.cpp"

template <uint_fast8_t N>
uint_fast32_t searchFunc(std::vector<uint8_t>&, uint_fast32_t, uint_fast8_t, const uint_fast8_t (&)[N]);

void	
	eraseSegments(std::vector<uint8_t>&, bool&),
	valueUpdater(std::vector<uint8_t>&, uint_fast32_t, const uint_fast32_t, uint_fast8_t);

uint_fast8_t imgPrmt(const std::string&);
