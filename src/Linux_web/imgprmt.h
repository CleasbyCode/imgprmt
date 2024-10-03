#pragma once

#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "searchFunc.cpp"
#include "eraseSegments.cpp"
#include "profileData.cpp"
#include "valueUpdater.cpp"
#include "imgprmt.cpp"

template <uint8_t N>
uint32_t searchFunc(std::vector<uint8_t>&, uint32_t, uint8_t, const uint8_t (&)[N]);

void	
	eraseSegments(std::vector<uint8_t>&, bool&),
	valueUpdater(std::vector<uint8_t>&, uint32_t, const uint32_t, uint8_t);

uint8_t imgPrmt(const std::string&);
