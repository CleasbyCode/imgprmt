int imgPrmt(const std::string& IMAGE_FILENAME, ArgOption platform) {
	std::ifstream image_ifs(IMAGE_FILENAME, std::ios::binary);

	if (!image_ifs) {
		std::cerr << "\nOpen File Error: Unable to read image file.\n\n";
		return 1;
	}

	const uintmax_t IMAGE_FILE_SIZE = std::filesystem::file_size(IMAGE_FILENAME);

	std::vector<uint8_t> image_vec;
	image_vec.resize(IMAGE_FILE_SIZE); 
	
	image_ifs.read(reinterpret_cast<char*>(image_vec.data()), IMAGE_FILE_SIZE);

	constexpr std::array<uint8_t, 2>
		SOI_SIG { 0xFF, 0xD8 },
		EOI_SIG { 0xFF, 0xD9 };

	if (!std::equal(SOI_SIG.begin(), SOI_SIG.end(), image_vec.begin()) || !std::equal(EOI_SIG.begin(), EOI_SIG.end(), image_vec.end() - 2)) {
        	std::cerr << "\nImage File Error: This is not a valid JPG image.\n\n";
		return 1;
	}
	
	bool hasBlueskyOption = (platform == ArgOption::Bluesky);

	eraseSegments(image_vec);
	
	std::string
		prompt,
		url_link;

	std::cout << "\n\nEnter a Web link (Image source, Social media page, etc.)\n\nURL: ";
	std::getline(std::cin, url_link);

	fflush(stdin); fflush(stdout);

	std::ignore = _setmode(_fileno(stdin), 0x20000);  // Set standard input for Windows to "_O_U16TEXT" so our wide strings contain correct wide characters.
	std::ignore = _setmode(_fileno(stdout), 0x20000); // Set standard output for Windows to "_O_U16TEXT" so the console displays the correct wide characters.

	std::wcout << L"\nType or paste in your prompt as one long sentence. Add <br> tags for new lines, if required.\n\nImage Description: ";

	constexpr uint32_t wcin_buffer_size = 8000;  	// The default string length in Windows console for "wcin" when "_O_U16TEXT" is set, 
							// seems to be just over 2000 characters. So we increase it here to 8000 to provide a more flexible size.

	wchar_t* wcin_buffer = new wchar_t[wcin_buffer_size]();

	std::wcin.rdbuf()->pubsetbuf(wcin_buffer, wcin_buffer_size); // Set the new buffer size, as above.

	std::wcin.getline(wcin_buffer, wcin_buffer_size);
	std::wstring wide_prompt(wcin_buffer);

	// Search the wide string for problem characters and replace them with html entity codes.
	replaceProblemChars(wide_prompt);

	// Determine the required buffer size for the multibyte string
	uint32_t bufferSize = WideCharToMultiByte(CP_UTF8, 0, wide_prompt.c_str(), -1, nullptr, 0, nullptr, nullptr);

	if (!bufferSize) {
		std::wcerr << L"Error: Unable to determine buffer size\n";
		return 1;
	}

	// Allocate memory for the multibyte string
	char* multiByteString = new char[bufferSize];

	// Convert the wide character string to multibyte (UTF-8) string
	if (WideCharToMultiByte(CP_UTF8, 0, wide_prompt.c_str(), -1, multiByteString, bufferSize, nullptr, nullptr) == 0) {
		std::wcerr << L"Error: Conversion failed\n";
		delete[] multiByteString;
		return 1;
	}

	delete[] wcin_buffer;

	constexpr uint16_t MAX_SEGMENT_SIZE = 10000;				// Segment size limit.

	const uint16_t
		PROMPT_INSERT_INDEX 	= hasBlueskyOption ? 0xE76 : 0xF3C,	// Insert location within Segment containing HTML page for the user's prompt text.
		LINK_INSERT_INDEX 	= hasBlueskyOption ? 0xDE7 : 0xEAD;	// Insert location within Segment containing HTML page for the user's web link.

	// For X/Twitter, Mastodon, Tumblr, Flickr.
	constexpr uint8_t 
		PROFILE_INTERNAL_DIFF 	= 38,	 // Bytes we don't count as part of internal profile size.
		PROFILE_MAIN_DIFF 	= 22,	 // Bytes we don't count as part of profile size.
		PROFILE_INSERT_INDEX 	= 0x14;	 // Insert location within Default_Vec for the color profile (Segment_Vec). X/Twitter, Mastodon, Tumblr, Flickr.
	
	uint8_t
		// For Twitter, Mastodon, Tumblr, Flickr.
		profile_size_field_index = 0x28, // Start index location for internal size field of the image color profile.(Max four bytes, only two used).

		segment_size_field_index = hasBlueskyOption ? 0x04 : 0x16,  // Start index location for size field of the main segment EXIF or color profile. (Max two bytes)

		exif_segment_xres_offset_field_index = 0x2A,
		exif_segment_xres_offset_size_diff = 0x36, // Always 0x36 size difference between EXIF segment size.

		exif_segment_yres_offset_field_index = 0x36,
		exif_segment_yres_offset_size_diff = 0x2E, // Always 0x2E size difference between EXIF segment size.

		exif_segment_artist_size_field_index = 0x4A,
		exif_segment_size_diff = 0x91, // EXIF segment size - artist comment section size.

		exif_segment_subifd_offset_index = 0x5A,
		exif_segment_subifd_offset_size_diff = 0x26, // Always 0x26 size difference between EXIF segment size.
		bits = 16;
		
	if (!hasBlueskyOption) {
		segment_vec.insert(segment_vec.begin() + PROFILE_INSERT_INDEX, profile_vec.begin(), profile_vec.end());
	} else { 
		segment_vec.swap(bluesky_vec);	
	}

	// Insert prompt & the url link into their relevant index positions of the html section, within vector Segment_Vec.
	segment_vec.insert(segment_vec.begin() + PROMPT_INSERT_INDEX, multiByteString, multiByteString + bufferSize - 1);
	segment_vec.insert(segment_vec.begin() + LINK_INSERT_INDEX, url_link.begin(), url_link.end());

	delete[] multiByteString;

	if (segment_vec.size() > MAX_SEGMENT_SIZE) {
		std::wcerr << L"\nFile Size Error: Data content size exceeds the maximum limit of 10KB.\n\n";
		return 1;
	}

	uint32_t segment_size = static_cast<uint32_t>(segment_vec.size()); 

	if (hasBlueskyOption) {
		// For Bluesky segment size don't count/include the JPG ID + APP ID "FFD8FFE1" (4 bytes).
		segment_size -= 4;

		// Update EXIF segment size field (FFE1xxxx)
		valueUpdater(segment_vec, segment_size_field_index, segment_size, bits);
		
		bits = 32;
		uint32_t 
			exif_xres_offset = segment_size - exif_segment_xres_offset_size_diff,
			exif_yres_offset = segment_size - exif_segment_yres_offset_size_diff,
			exif_artist_size = (segment_size - exif_segment_size_diff) + 4, // For this variable, include the JPG ID + APP ID "FFD8FFE1" (4 bytes).
			exif_subifd_offset = segment_size - exif_segment_subifd_offset_size_diff;
			
		valueUpdater(segment_vec, exif_segment_xres_offset_field_index, exif_xres_offset, bits);
		valueUpdater(segment_vec, exif_segment_yres_offset_field_index, exif_yres_offset, bits);
		valueUpdater(segment_vec, exif_segment_artist_size_field_index, exif_artist_size, bits); 
		valueUpdater(segment_vec, exif_segment_subifd_offset_index, exif_subifd_offset, bits);
	} else {
		// Update color profile segment size field (FFE2xxxx)
		valueUpdater(segment_vec, segment_size_field_index, segment_size - PROFILE_MAIN_DIFF, bits);

		// Update internal color profile size field
		valueUpdater(segment_vec, profile_size_field_index, segment_size - PROFILE_INTERNAL_DIFF, bits);
	}

	image_vec.insert(image_vec.begin(), segment_vec.begin(), segment_vec.end());
	
	// Clear	
	std::vector<uint8_t>().swap(segment_vec);

	const uint32_t IMAGE_SIZE = static_cast<uint32_t>(image_vec.size());

	srand((unsigned)time(NULL)); 

	const std::wstring 
		TIME_VALUE = std::to_wstring(rand()),
		OUTPUT_FILENAME = L"imgprmt_" + TIME_VALUE.substr(0, 5) + L".jpg";

	std::ofstream file_ofs(OUTPUT_FILENAME, std::ios::binary);

	if (!file_ofs) {
		std::wcerr << L"\nWrite Error: Unable to write to file.\n\n";
		return 1;
	}
	file_ofs.write(reinterpret_cast<const char*>(image_vec.data()), IMAGE_SIZE);
	std::vector<uint8_t>().swap(image_vec);
	std::wcout << L"\nCreated output file: \"" + OUTPUT_FILENAME + L" " << IMAGE_SIZE << " " << "Bytes\"\n\n";
        return 0;
}