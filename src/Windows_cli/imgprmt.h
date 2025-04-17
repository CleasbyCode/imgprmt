#pragma once

#include <algorithm>
#include <array>
#include <set>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

// This software is based in part on the work of the Independent JPEG Group.
// https://github.com/libjpeg-turbo/libjpeg-turbo
// Copyright (C) 2009-2024 D. R. Commander. All Rights Reserved.
// Copyright (C) 2015 Viktor Szathmáry. All Rights Reserved.
#include "libjpeg-turbo/turbojpeg.h"

#define NOMINMAX
#include <windows.h>

#include <io.h>

#include "information.cpp"
#include "programArgs.cpp"
#include "fileChecks.cpp"
#include "searchFunc.cpp"
#include "transcodeImage.cpp"
#include "eraseSegments.cpp"
#include "replaceChars.cpp"
#include "segmentData.cpp"
#include "valueUpdater.cpp"
#include "imgprmt.cpp"

template <typename T, size_t N>
uint32_t searchFunc(std::vector<uint8_t>&, uint32_t, const uint8_t, const std::array<T, N>&);

bool hasValidFilename(const std::string&);

void
	validateFiles(const std::string&),
	transcodeImage(std::vector<uint8_t>&, bool),
	eraseSegments(std::vector<uint8_t>&),
	replaceProblemChars(std::wstring& str),
	valueUpdater(std::vector<uint8_t>&, uint32_t, const uint32_t, uint8_t),
	displayInfo();

int imgPrmt(const std::string&, ArgOption);
