//  imgprmt v1.8 (Linux / Windows CLI Edition). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023

// Compile program (Linux):

// $ sudo apt-get install libturbojpeg0-dev

// $ chmod +x compile_imgprmt.sh
// $ ./compile_imgprmt.sh
	
// $ Compilation successful. Executable 'imgprmt' created.
// $ sudo cp imgprmt /usr/bin
// $ imgprmt

#include <turbojpeg.h>

// This software is based in part on the work of the Independent JPEG Group.
// Copyright (C) 2009-2024 D. R. Commander. All Rights Reserved.
// Copyright (C) 2015 Viktor Szathmáry. All Rights Reserved.
// https://github.com/libjpeg-turbo/libjpeg-turbo

#ifdef _WIN32
	#define NOMINMAX
    #include <windows.h>

   	#include <fcntl.h>
    #include <io.h>
    #include <tuple>
    #include <cstdio>
#else
    #include <cwchar>
#endif

#include "fileChecks.h"
#include "segmentsVec.h"

#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <array>
#include <algorithm>
#include <random>
#include <locale>

// Return vector index location for relevant signature search.
template <typename T, size_t N>
static inline uint32_t searchSig(std::vector<uint8_t>& vec, const std::array<T, N>& SIG) {
	return static_cast<uint32_t>(std::search(vec.begin(), vec.end(), SIG.begin(), SIG.end()) - vec.begin());
}

// Writes updated values, such as segments lengths, index/offsets values, etc. into the relevant vector index location.	
static inline void updateValue(std::vector<uint8_t>& vec, uint32_t insert_index, uint32_t NEW_VALUE, uint8_t bits) {
	while (bits) {
		vec[insert_index++] = (NEW_VALUE >> (bits -= 8)) & 0xFF; // Big-endian.
    }
}

static inline void replaceProblemChars(std::wstring& str) {
	constexpr uint8_t ARR_SIZE = 4;
    constexpr std::array<wchar_t, ARR_SIZE> Tag{ 96, 39, 13, 10 }; 

    for (int i = static_cast<int>(str.length()) - 1; i >= 0; --i) {
    	wchar_t c = str[i];
        for (wchar_t tagChar : Tag) {
        	if (c == tagChar) {
            	std::wstring html_entity = L"&#" + std::to_wstring(tagChar) + L";";
                str.erase(str.begin() + i);
                str.insert(str.begin() + i, html_entity.begin(), html_entity.end());
                break; 
            }
        }
    }
}

static inline std::string convertString(const std::wstring& wide) {
	#ifdef _WIN32
    	if (wide.empty()) return {};

    	int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
    	if (size <= 0) throw std::runtime_error("WideCharToMultiByte failed.");

    	std::string convertedString(size - 1, 0); // Exclude null terminator
    	WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, convertedString.data(), size, nullptr, nullptr);
    	return convertedString;

	#else
    	std::mbstate_t state{};
    	const wchar_t* src = wide.data();
    	size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
    	if (len == static_cast<size_t>(-1)) throw std::runtime_error("Conversion to UTF-8 failed.");

    	std::string convertedString(len, 0);
    	src = wide.data(); // reset pointer
    	std::wcsrtombs(convertedString.data(), &src, len, &state);
    	return convertedString;
	#endif
}

int main(int argc, char** argv) {
	try {
		ProgramArgs args = ProgramArgs::parse(argc, argv);
		
		std::vector<uint8_t> cover_image_vec;
		
		uintmax_t cover_image_size = 0;
		
		validateImageFile(args.cover_image, args.platform, cover_image_size, cover_image_vec);

		// For improved compatibility, default re-encode image to JPG Progressive format with a quality value set at 97 with no chroma subsampling,
		// or if Bluesky option, re-encode to standard Baseline format with a quality value set at 85.
	
		// -------------
			
		tjhandle decompressor = tjInitDecompress();
    	if (!decompressor) {
        	throw std::runtime_error("tjInitDecompress() failed.");
    	}

    	int width = 0, height = 0, jpegSubsamp = 0, jpegColorspace = 0;
    	if (tjDecompressHeader3(decompressor, cover_image_vec.data(), static_cast<unsigned long>(cover_image_vec.size()), &width, &height, &jpegSubsamp, &jpegColorspace) != 0) {
        	tjDestroy(decompressor);
        	throw std::runtime_error(std::string("tjDecompressHeader3: ") + tjGetErrorStr());
    	}

    	std::vector<uint8_t> decoded_image_vec(width * height * 3); 
    	if (tjDecompress2(decompressor, cover_image_vec.data(),static_cast<unsigned long>(cover_image_vec.size()), decoded_image_vec.data(), width, 0, height, TJPF_RGB, 0) != 0) {
        	tjDestroy(decompressor);
        	throw std::runtime_error(std::string("tjDecompress2: ") + tjGetErrorStr());
    	}
    	tjDestroy(decompressor);
    	tjhandle compressor = tjInitCompress();
    	if (!compressor) {
        	throw std::runtime_error("tjInitCompress() failed.");
    	}

    	const uint8_t JPG_QUALITY_VAL = (args.platform == ArgOption::Bluesky) ? 85 : 97;

    	uint8_t* jpegBuf = nullptr;
    	unsigned long jpegSize = 0;

    	int flags = TJFLAG_ACCURATEDCT | ((args.platform == ArgOption::Bluesky) ? 0 : TJFLAG_PROGRESSIVE);

    	if (tjCompress2(compressor, decoded_image_vec.data(), width, 0, height, TJPF_RGB, &jpegBuf, &jpegSize, TJSAMP_444, JPG_QUALITY_VAL, flags) != 0) {
        	tjDestroy(compressor);
        	throw std::runtime_error(std::string("tjCompress2: ") + tjGetErrorStr());
    	}
    	tjDestroy(compressor);

    	std::vector<uint8_t> output_image_vec(jpegBuf, jpegBuf + jpegSize);
    	tjFree(jpegBuf);
    			
    	cover_image_vec.swap(output_image_vec);
    			
    	std::vector<uint8_t>().swap(output_image_vec);
    	std::vector<uint8_t>().swap(decoded_image_vec);
    			
		// ------------
	
		// Remove superfluous segments from cover image. (EXIF, ICC color profile, etc).
		constexpr std::array<uint8_t, 2>
			APP1_SIG { 0xFF, 0xE1 }, // EXIF SEGMENT MARKER.
			APP2_SIG { 0xFF, 0xE2 }; // ICC COLOR PROFILE SEGMENT MARKER.

		constexpr std::array<uint8_t, 4>
			DQT1_SIG { 0xFF, 0xDB, 0x00, 0x43 },
			DQT2_SIG { 0xFF, 0xDB, 0x00, 0x84 };
		
		const uint32_t APP1_POS = searchSig(cover_image_vec, APP1_SIG);

		if (cover_image_vec.size() > APP1_POS) {
			const uint16_t APP1_BLOCK_SIZE = (static_cast<uint16_t>(cover_image_vec[APP1_POS + 2]) << 8) | static_cast<uint16_t>(cover_image_vec[APP1_POS + 3]);
			cover_image_vec.erase(cover_image_vec.begin() + APP1_POS, cover_image_vec.begin() + APP1_POS + APP1_BLOCK_SIZE + 2);
		}

		const uint32_t APP2_POS = searchSig(cover_image_vec, APP2_SIG);
		if (cover_image_vec.size() > APP2_POS) {
			const uint16_t APP2_BLOCK_SIZE = (static_cast<uint16_t>(cover_image_vec[APP2_POS + 2]) << 8) | static_cast<uint16_t>(cover_image_vec[APP2_POS + 3]);
			cover_image_vec.erase(cover_image_vec.begin() + APP2_POS, cover_image_vec.begin() + APP2_POS + APP2_BLOCK_SIZE + 2);
		}

		const uint32_t
			DQT1_POS = searchSig(cover_image_vec, DQT1_SIG),
			DQT2_POS = searchSig(cover_image_vec, DQT2_SIG),
			DQT_POS  = std::min(DQT1_POS, DQT2_POS);

		cover_image_vec.erase(cover_image_vec.begin(), cover_image_vec.begin() + DQT_POS);
		// ------------
		
		std::cout << "\n*** imgprmt v1.8 ***\n";

		#ifdef _WIN32
			constexpr uint16_t WIN_BUFFER_SIZE  = 65535;
			wchar_t* buffer = new wchar_t[WIN_BUFFER_SIZE];
			std::wcin.rdbuf()->pubsetbuf(buffer, WIN_BUFFER_SIZE);

			// Store original modes
			int original_stdin_mode = _setmode(_fileno(stdin), _O_BINARY);  	// temporarily switch to binary
			_setmode(_fileno(stdin), original_stdin_mode); 						// restore original immediately after saving

			int original_stdout_mode = _setmode(_fileno(stdout), _O_BINARY);  	// temporarily switch to binary
			_setmode(_fileno(stdout), original_stdout_mode); 					// restore original immediately after saving

			fflush(stdin);
			fflush(stdout);

			std::ignore = _setmode(_fileno(stdin), 0x20000); 					// set text mode to UTF-16
			std::ignore = _setmode(_fileno(stdout), 0x20000);

		#else
			// Linux
			std::locale::global(std::locale("en_US.UTF-8")); 

			std::wcin.imbue(std::locale());
			std::wcout.imbue(std::locale());
		#endif
	
		std::wstring
			wprompt,
			wurl;

		std::wcout << L"\nEnter a Web link (Image source, Social media page, etc.)\n\nFull URL Address: ";
		std::getline(std::wcin, wurl);
	
		const std::wstring prefix = L"https://";
	
		if (wurl.length() < 12 || wurl.substr(0, prefix.length()) != prefix) {
			throw std::runtime_error("Link Error: URL must start with 'https://' and be at least 12 characters long.");
		}
	
		uint16_t platform_char_limit;
	
		#ifdef _WIN32
			constexpr uint16_t
				WIN_MAX_CHAR = 59392,
				TWITTER_MAX_CHAR = 6300;	
			
			platform_char_limit = (args.platform == ArgOption::Twitter) ? TWITTER_MAX_CHAR : WIN_MAX_CHAR;
		#else
			platform_char_limit = 4096;
		#endif

		std::wcout << L"\nType or paste in your prompt as one long sentence. Character limit: " << platform_char_limit << ".\n";
		std::wcout << L"\nAvoid newline characters, instead add <br> tags for new lines, if required.\n\nImage Description: ";
		std::getline(std::wcin, wprompt);

		replaceProblemChars(wprompt);

		std::string utf8_url = convertString(wurl);
		std::wstring().swap(wurl);
		
		std::string utf8_prompt = convertString(wprompt);
		std::wstring().swap(wprompt);
		
		const uint16_t
			// Insert location within vector containing HTML page for the user's prompt text.
			PROMPT_INSERT_INDEX = (args.platform == ArgOption::Bluesky) ? 0xF40 : ((args.platform == ArgOption::Twitter) ? 0xE04 : 0x1066),	
			// Insert location within vector containing HTML page for the user's web link.
			LINK_INSERT_INDEX   = (args.platform == ArgOption::Bluesky) ? 0xEC5 : ((args.platform == ArgOption::Twitter) ? 0xDC4 : 0xFEB);	

		// For X-Twitter, Mastodon, Tumblr, Flickr.
		constexpr uint8_t
			PROFILE_INTERNAL_DIFF 	 = 38,	 // Bytes we don't count as part of internal profile size.
			PROFILE_MAIN_DIFF 	 	 = 22,	 // Bytes we don't count as part of profile size.
			PROFILE_INSERT_INDEX 	 = 0x14, // Insert location within Default_Vec for the color profile (Segment_Vec). X/Twitter, Mastodon, Tumblr, Flickr.
			PROFILE_SIZE_FIELD_INDEX = 0x28, // Start index location for internal size field of the image color profile.(Max four bytes, only two used).
			SEGMENT_SIZE_FIELD_INDEX = 0x16; // Start index location for size field of the main color profile. (Max two bytes)
		
		uint8_t bits = 16;

		if (args.platform == ArgOption::Twitter) {
			args.platform = ArgOption::Default;  
			segment_vec.swap(twitter_vec);
		}

		if (args.platform == ArgOption::Default) { 
			segment_vec.insert(segment_vec.begin() + PROFILE_INSERT_INDEX, profile_vec.begin(), profile_vec.end());
		} else {
			segment_vec.swap(bluesky_vec);
		}

		// Clear vectors we no longer need.
		std::vector<uint8_t>().swap(twitter_vec); 
		std::vector<uint8_t>().swap(bluesky_vec);

		// Insert prompt & the url link into their relevant index positions of the html section, within vector Segment_Vec.
		segment_vec.insert(segment_vec.begin() + PROMPT_INSERT_INDEX, utf8_prompt.begin(), utf8_prompt.end());
		std::string().swap(utf8_prompt);
		
		segment_vec.insert(segment_vec.begin() + LINK_INSERT_INDEX, utf8_url.begin(), utf8_url.end());
		std::string().swap(utf8_url);

		#ifdef _WIN32
			constexpr uint16_t TWITTER_SEGMENT_LIMIT = 10 * 1024;
			const uint16_t MAX_SEGMENT_SIZE = (args.platform == ArgOption::Twitter) ? TWITTER_SEGMENT_LIMIT : WIN_BUFFER_SIZE;  // Twitter 10KB, Other ~64KB.

			if (segment_vec.size() > MAX_SEGMENT_SIZE) {
				throw std::runtime_error("File Size Error: Data content size exceeds the maximum segment limit.");
			}
		#endif

		uint32_t segment_size = static_cast<uint32_t>(segment_vec.size());

		if (args.platform == ArgOption::Bluesky) {
			segment_size -= 4; // For Bluesky segment size, don't count the JPG ID + APP ID "FFD8FFE1" (4 bytes).
		
			constexpr uint8_t 
				EXIF_SIZE_FIELD_INDEX 			= 0x04,  
				EXIF_XRES_OFFSET_FIELD_INDEX 	= 0x2A,  
				EXIF_YRES_OFFSET_FIELD_INDEX 	= 0x36,  
				EXIF_ARTIST_SIZE_FIELD_INDEX 	= 0x4A,  
				EXIF_SUBIFD_OFFSET_FIELD_INDEX 	= 0x5A;  

			const uint16_t	
				EXIF_XRES_OFFSET   = segment_size - 0x36,
				EXIF_YRES_OFFSET   = segment_size - 0x2E,
				EXIF_SUBIFD_OFFSET = segment_size - 0x26,
				EXIF_ARTIST_SIZE   = segment_size - 0x8D;

			updateValue(segment_vec, EXIF_SIZE_FIELD_INDEX , segment_size, bits);
		
			bits = 32;

			updateValue(segment_vec, EXIF_XRES_OFFSET_FIELD_INDEX, EXIF_XRES_OFFSET, bits);
			updateValue(segment_vec, EXIF_YRES_OFFSET_FIELD_INDEX, EXIF_YRES_OFFSET, bits);
			updateValue(segment_vec, EXIF_ARTIST_SIZE_FIELD_INDEX, EXIF_ARTIST_SIZE, bits); 
			updateValue(segment_vec, EXIF_SUBIFD_OFFSET_FIELD_INDEX, EXIF_SUBIFD_OFFSET, bits);
		} else {
			// Update color profile segment size field (FFE2xxxx)
			updateValue(segment_vec, SEGMENT_SIZE_FIELD_INDEX, segment_size - PROFILE_MAIN_DIFF, bits);

			// Update internal color profile size field
			updateValue(segment_vec, PROFILE_SIZE_FIELD_INDEX, segment_size - PROFILE_INTERNAL_DIFF, bits);
		}	

		cover_image_vec.insert(cover_image_vec.begin(), segment_vec.begin(), segment_vec.end());
		std::vector<uint8_t>().swap(segment_vec); // Clear

		#ifdef _WIN32
			_setmode(_fileno(stdin), original_stdin_mode);
			_setmode(_fileno(stdout), original_stdout_mode);
		#endif

		cover_image_size = static_cast<uint32_t>(cover_image_vec.size());

		std::random_device rd;
    		std::mt19937 gen(rd());
    		std::uniform_int_distribution<> dist(10000, 99999);
	
		const std::string OUTPUT_FILENAME = "imgprmt_" + std::to_string(dist(gen)) + ".jpg";

		std::ofstream write_file_fs(OUTPUT_FILENAME, std::ios::binary);
		if (!write_file_fs) {
			throw std::runtime_error("Write Error: Unable to write to file. Make sure you have WRITE permissions for this location.");
		}

		write_file_fs.write(reinterpret_cast<const char*>(cover_image_vec.data()), cover_image_size);
		
		std::vector<uint8_t>().swap(cover_image_vec);
		std::cout << "\nSaved \"prompt-embedded\" JPG image: " << OUTPUT_FILENAME << " (" << cover_image_size << " bytes).\n\n";
		return 0;
	}
	catch (const std::runtime_error& e) {
    	std::cerr << "\n" << e.what() << "\n\n";
        return 1;
    }
}

