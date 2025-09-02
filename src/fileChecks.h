#pragma once

#include "programArgs.h"
#include <vector>
#include <cstdint>

bool hasValidImageExtension(const std::string&);
bool hasValidFilename(const std::string&);
void validateImageFile(std::string&, ArgOption, uintmax_t&, std::vector<uint8_t>&);
