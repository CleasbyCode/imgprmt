#include "imgPrmt.h"
#include "segmentsVec.h"
#include "transcodeImage.h"
#include "replaceChars.h"
#include "eraseSegments.h"
#include "convertString.h"

#include <iostream>
#include <csignal>
#include <fstream>
#include <array>
#include <algorithm>
#include <filesystem>
#include <random>
#include <locale>

#ifdef _WIN32
    #include <fcntl.h>
    #include <io.h>
    #include <cstdio>
#endif

int imgPrmt(const std::string& IMAGE_FILENAME, ArgOption platformOption) {
	// Small Lambda function used multiple times in this function. 
	// Writes updated values (2 bytes or 4 bytes), such as segments lengths, index/offsets values, etc. into the relevant vector index location.	
	auto updateValue = [](std::vector<uint8_t>& vec, uint32_t insert_index, const uint32_t NEW_VALUE, uint8_t bits) {
		while (bits) { vec[insert_index++] = (NEW_VALUE >> (bits -= 8)) & 0xff; }	// Big-endian.
		};

	const size_t IMAGE_FILE_SIZE = std::filesystem::file_size(IMAGE_FILENAME);

	std::ifstream image_file_ifs(IMAGE_FILENAME, std::ios::binary);

	if (!image_file_ifs) {
		std::cerr << "\nRead File Error: Unable to read.\n\n";
		return 1;
	}

	std::vector<uint8_t> Image_Vec(IMAGE_FILE_SIZE);

	image_file_ifs.read(reinterpret_cast<char*>(Image_Vec.data()), IMAGE_FILE_SIZE);
	image_file_ifs.close();

	constexpr uint8_t SIG_LENGTH = 2;

	constexpr std::array<uint8_t, SIG_LENGTH>
		IMAGE_START_SIG	{ 0xFF, 0xD8 },
		IMAGE_END_SIG   { 0xFF, 0xD9 };

	if (!std::equal(IMAGE_START_SIG.begin(), IMAGE_START_SIG.end(), Image_Vec.begin()) || !std::equal(IMAGE_END_SIG.begin(), IMAGE_END_SIG.end(), Image_Vec.end() - 2)) {
        	std::cerr << "\nImage File Error: This is not a valid JPG image.\n\n";
		return 1;
	}
	
	bool
		hasBlueSkyOption = (platformOption == ArgOption::Bluesky),
		hasTwitterOption = (platformOption == ArgOption::Twitter),
		hasDefaultOption = (platformOption == ArgOption::Default);

	// For better compatibility, default re-encode image to JPG Progressive format with a quality value set at 97 with no chroma subsampling.
	// If Bluesky option, re-encode to standard Baseline format with a quality value set at 85.
	transcodeImage(Image_Vec, hasBlueSkyOption);

	eraseSegments(Image_Vec);
	
	std::cout << "\n*** imgprmt v1.7 ***\n";

	#ifdef _WIN32
		constexpr uint16_t WIN_BUFFER_SIZE  = 65535;
		wchar_t* buffer = new wchar_t[WIN_BUFFER_SIZE];
		std::wcin.rdbuf()->pubsetbuf(buffer, WIN_BUFFER_SIZE);

		// Store original modes
		int original_stdin_mode = _setmode(_fileno(stdin), _O_BINARY);  // temporarily switch to binary
		_setmode(_fileno(stdin), original_stdin_mode); // restore original immediately after saving

		int original_stdout_mode = _setmode(_fileno(stdout), _O_BINARY);  // temporarily switch to binary
		_setmode(_fileno(stdout), original_stdout_mode); // restore original immediately after saving

		fflush(stdin);
		fflush(stdout);

		std::ignore = _setmode(_fileno(stdin), 0x20000); // set text mode to UTF-16
		std::ignore = _setmode(_fileno(stdout), 0x20000);

	#else
		// Linux.
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
		std::wcerr << L"\nLink Error: URL must start with 'https://' and be at least 12 characters long.\n\n";
		return 1;
	}
	
	uint16_t platform_char_limit;
	
	#ifdef _WIN32
		constexpr uint16_t
			WIN_MAX_CHAR = 59392,
			TWITTER_MAX_CHAR = 6300;	
		platform_char_limit = hasTwitterOption ? TWITTER_MAX_CHAR : WIN_MAX_CHAR;
	#else
		platform_char_limit = 4096;
	#endif

	std::wcout << L"\nType or paste in your prompt as one long sentence. Character limit: " << platform_char_limit << ".\n";
	std::wcout << L"\nAvoid newline characters, instead add <br> tags for new lines, if required.\n\nImage Description: ";
	std::getline(std::wcin, wprompt);

	replaceProblemChars(wprompt);

	std::string utf8_url = convertString(wurl);
	std::string utf8_prompt = convertString(wprompt);

	const uint16_t
		PROMPT_INSERT_INDEX = hasBlueSkyOption ? 0xF40 : (hasTwitterOption ? 0xE04 : 0x1066),	// Insert location within vector containing HTML page for the user's prompt text.
		LINK_INSERT_INDEX   = hasBlueSkyOption ? 0xEC5 : (hasTwitterOption ? 0xDC4 : 0xFEB);	// Insert location within vector containing HTML page for the user's web link.

	// For X-Twitter, Mastodon, Tumblr, Flickr.
	constexpr uint8_t
		PROFILE_INTERNAL_DIFF 	= 38,	 // Bytes we don't count as part of internal profile size.
		PROFILE_MAIN_DIFF 	= 22,	 // Bytes we don't count as part of profile size.
		PROFILE_INSERT_INDEX 	= 0x14;	 // Insert location within Default_Vec for the color profile (Segment_Vec). X/Twitter, Mastodon, Tumblr, Flickr.

	uint8_t
		// For Twitter, Mastodon, Tumblr, Flickr.
		profile_size_field_index = 0x28, // Start index location for internal size field of the image color profile.(Max four bytes, only two used).

		segment_size_field_index = hasBlueSkyOption ? 0x04 : 0x16,  // Start index location for size field of the main segment EXIF or color profile. (Max two bytes)
		exif_segment_xres_offset_field_index = 0x2A,
		exif_segment_xres_offset_size_diff = 0x36, // Always 0x36 size difference between EXIF segment size.

		exif_segment_yres_offset_field_index = 0x36,
		exif_segment_yres_offset_size_diff = 0x2E, // Always 0x2E size difference between EXIF segment size.

		exif_segment_artist_size_field_index = 0x4A,
		exif_segment_size_diff = 0x91, // EXIF segment size - artist comment section size.

		exif_segment_subifd_offset_index = 0x5A,
		exif_segment_subifd_offset_size_diff = 0x26, // Always 0x26 size difference between EXIF segment size.
		bits = 16;

	if (hasTwitterOption) {
		hasDefaultOption = true;  
		Segment_Vec.swap(Twitter_Vec);
	}

	if (hasDefaultOption) { 
		Segment_Vec.insert(Segment_Vec.begin() + PROFILE_INSERT_INDEX, Profile_Vec.begin(), Profile_Vec.end());
	} else {
		Segment_Vec.swap(BlueSky_Vec);
	}

	// Insert prompt & the url link into their relevant index positions of the html section, within vector Segment_Vec.
	Segment_Vec.insert(Segment_Vec.begin() + PROMPT_INSERT_INDEX, utf8_prompt.begin(), utf8_prompt.end());
	Segment_Vec.insert(Segment_Vec.begin() + LINK_INSERT_INDEX, utf8_url.begin(), utf8_url.end());

	#ifdef _WIN32
		constexpr uint16_t TWITTER_SEGMENT_LIMIT = 10 * 1024;
		const uint16_t MAX_SEGMENT_SIZE = hasTwitterOption ? TWITTER_SEGMENT_LIMIT : WIN_BUFFER_SIZE;  // Twitter 10KB, Other ~64KB.

		if (Segment_Vec.size() > MAX_SEGMENT_SIZE) {
			std::wcerr << L"\nFile Size Error: Data content size exceeds the maximum segment limit.\n\n";
			return 1;
		}
	#endif

	uint32_t segment_size = static_cast<uint32_t>(Segment_Vec.size());

	if (hasBlueSkyOption) {
		// For Bluesky segment size don't count/include the JPG ID + APP ID "FFD8FFE1" (4 bytes).
		segment_size -= 4;

		// Update EXIF segment size field (FFE1xxxx)
		updateValue(Segment_Vec, segment_size_field_index, segment_size, bits);

		bits = 32;
		uint32_t
			exif_xres_offset = segment_size - exif_segment_xres_offset_size_diff,
			exif_yres_offset = segment_size - exif_segment_yres_offset_size_diff,
			exif_artist_size = (segment_size - exif_segment_size_diff) + 4, // For this variable, include the JPG ID + APP ID "FFD8FFE1" (4 bytes).
			exif_subifd_offset = segment_size - exif_segment_subifd_offset_size_diff;

		updateValue(Segment_Vec, exif_segment_xres_offset_field_index, exif_xres_offset, bits);
		updateValue(Segment_Vec, exif_segment_yres_offset_field_index, exif_yres_offset, bits);
		updateValue(Segment_Vec, exif_segment_artist_size_field_index, exif_artist_size, bits);
		updateValue(Segment_Vec, exif_segment_subifd_offset_index, exif_subifd_offset, bits);
	} else {
		// Update color profile segment size field (FFE2xxxx)
		updateValue(Segment_Vec, segment_size_field_index, segment_size - PROFILE_MAIN_DIFF, bits);

		// Update internal color profile size field
		updateValue(Segment_Vec, profile_size_field_index, segment_size - PROFILE_INTERNAL_DIFF, bits);
	}

	Image_Vec.insert(Image_Vec.begin(), Segment_Vec.begin(), Segment_Vec.end());

#ifdef _WIN32
	_setmode(_fileno(stdin), original_stdin_mode);
    _setmode(_fileno(stdout), original_stdout_mode);
#endif

	const uint32_t IMAGE_SIZE = static_cast<uint32_t>(Image_Vec.size());

	std::random_device rd;
    	std::mt19937 gen(rd());
    	std::uniform_int_distribution<> dist(10000, 99999);
	
	const std::string OUTPUT_FILENAME = "imgprmt_" + std::to_string(dist(gen)) + ".jpg";

	std::ofstream write_file_fs(OUTPUT_FILENAME, std::ios::binary);
	if (!write_file_fs) {
		std::cerr << "\nWrite Error: Unable to write to file.\n\n";
		return 1;
	}

	write_file_fs.write(reinterpret_cast<const char*>(Image_Vec.data()), IMAGE_SIZE);
	std::vector<uint8_t>().swap(Image_Vec);
	std::cout << "\nSaved \"prompt-embedded\" JPG image: " << OUTPUT_FILENAME << " (" << IMAGE_SIZE << " bytes).\n\n";
	return 0;
}
