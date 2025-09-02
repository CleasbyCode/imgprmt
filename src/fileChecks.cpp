#include "fileChecks.h"
#include <set>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <filesystem>
#include <fstream> 
#include <array>
#include <iostream>

bool hasValidFilename(const std::string& filename) {
	return std::all_of(filename.begin(), filename.end(), [](char c) { return std::isalnum(c) || c == '.' || c == '/' || c == '\\' || c == '-' || c == '_' || c == '@' || c == '%'; });
}

bool hasValidImageExtension(const std::string& ext) {
	static const std::set<std::string> valid_extensions = {".jpg", ".jpeg", ".jfif"};
    	return valid_extensions.count(ext) > 0;
}

void validateImageFile(std::string& cover_image, ArgOption platform, uintmax_t& cover_image_size, std::vector<uint8_t>& cover_image_vec) {
	std::filesystem::path image_path(cover_image);

    	std::string image_ext = image_path.extension().string();

	if (!std::filesystem::exists(image_path)) {
        	throw std::runtime_error("Image File Error: File not found.");
    	}

	if (!hasValidFilename(image_path.string())) {
    		throw std::runtime_error("Invalid Input Error: Unsupported characters in filename arguments.");
    	}

    	if (!hasValidImageExtension(image_ext)) {
        	throw std::runtime_error("File Type Error: Invalid image extension. Only expecting \".jpg\", \".jpeg\", or \".jfif\".");
    	}

	std::ifstream cover_image_ifs(image_path, std::ios::binary);
        	
    	if (!cover_image_ifs) {
    		throw std::runtime_error("Read File Error: Unable to read image file. Check the filename and try again.");
	}

	cover_image_size = std::filesystem::file_size(image_path);

    	constexpr uint8_t MINIMUM_IMAGE_SIZE = 134;

    	if (MINIMUM_IMAGE_SIZE > cover_image_size) {
        	throw std::runtime_error("Image File Error: Invalid file size.");
    	}

    	constexpr uintmax_t MAX_IMAGE_SIZE = 5ULL * 1024 * 1024;    

	if (cover_image_size > MAX_IMAGE_SIZE) {
		throw std::runtime_error("File Size Error: Image file exceeds maximum size limit.");
	}
	
	std::vector<uint8_t> tmp_vec(cover_image_size);
	
	cover_image_ifs.read(reinterpret_cast<char*>(tmp_vec.data()), cover_image_size);
	cover_image_ifs.close();
	
	cover_image_vec.swap(tmp_vec);
	std::vector<uint8_t>().swap(tmp_vec);
	
	constexpr std::array<uint8_t, 2>
		IMAGE_START_SIG	{ 0xFF, 0xD8 },
		IMAGE_END_SIG   { 0xFF, 0xD9 };

	if (!std::equal(IMAGE_START_SIG.begin(), IMAGE_START_SIG.end(), cover_image_vec.begin()) || !std::equal(IMAGE_END_SIG.begin(), IMAGE_END_SIG.end(), cover_image_vec.end() - 2)) {
    		throw std::runtime_error("Image File Error: This is not a valid JPG image.");
	}
}

