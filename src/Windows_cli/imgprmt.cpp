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

	std::ignore = _setmode(_fileno(stdin), 0x20000);  
	std::ignore = _setmode(_fileno(stdout), 0x20000); 

	std::wcout << L"\nType or paste in your prompt as one long sentence. Add <br> tags for new lines, if required.\n\nImage Description: ";

	constexpr uint32_t wcin_buffer_size = 8000;  	
							
	wchar_t* wcin_buffer = new wchar_t[wcin_buffer_size]();

	std::wcin.rdbuf()->pubsetbuf(wcin_buffer, wcin_buffer_size); 

	std::wcin.getline(wcin_buffer, wcin_buffer_size);
	std::wstring wide_prompt(wcin_buffer);

	replaceProblemChars(wide_prompt);

	uint32_t bufferSize = WideCharToMultiByte(CP_UTF8, 0, wide_prompt.c_str(), -1, nullptr, 0, nullptr, nullptr);

	if (!bufferSize) {
		std::wcerr << L"Error: Unable to determine buffer size\n";
		return 1;
	}

	char* multiByteString = new char[bufferSize];

	if (WideCharToMultiByte(CP_UTF8, 0, wide_prompt.c_str(), -1, multiByteString, bufferSize, nullptr, nullptr) == 0) {
		std::wcerr << L"Error: Conversion failed\n";
		delete[] multiByteString;
		return 1;
	}

	delete[] wcin_buffer;

	constexpr uint16_t MAX_SEGMENT_SIZE = 10000;

	const uint16_t
		PROMPT_INSERT_INDEX 	= hasBlueskyOption ? 0xE76 : 0xF3C,	
		LINK_INSERT_INDEX 	= hasBlueskyOption ? 0xDE7 : 0xEAD;	

	constexpr uint8_t 
		PROFILE_INTERNAL_DIFF 	= 38,	 
		PROFILE_MAIN_DIFF 	= 22,	 
		PROFILE_INSERT_INDEX 	= 0x14;	 
	
	uint8_t
		profile_size_field_index = 0x28, 
		segment_size_field_index = hasBlueskyOption ? 0x04 : 0x16,  
		exif_segment_xres_offset_field_index = 0x2A,
		exif_segment_xres_offset_size_diff = 0x36, 
		exif_segment_yres_offset_field_index = 0x36,
		exif_segment_yres_offset_size_diff = 0x2E, 
		exif_segment_artist_size_field_index = 0x4A,
		exif_segment_size_diff = 0x91, 
		exif_segment_subifd_offset_index = 0x5A,
		exif_segment_subifd_offset_size_diff = 0x26, 
		bits = 16;
		
	if (!hasBlueskyOption) {
		segment_vec.insert(segment_vec.begin() + PROFILE_INSERT_INDEX, profile_vec.begin(), profile_vec.end());
	} else { 
		segment_vec.swap(bluesky_vec);	
	}

	segment_vec.insert(segment_vec.begin() + PROMPT_INSERT_INDEX, multiByteString, multiByteString + bufferSize - 1);
	segment_vec.insert(segment_vec.begin() + LINK_INSERT_INDEX, url_link.begin(), url_link.end());

	delete[] multiByteString;

	if (segment_vec.size() > MAX_SEGMENT_SIZE) {
		std::wcerr << L"\nFile Size Error: Data content size exceeds the maximum limit of 10KB.\n\n";
		return 1;
	}

	uint32_t segment_size = static_cast<uint32_t>(segment_vec.size()); 

	if (hasBlueskyOption) {
		segment_size -= 4;
		
		valueUpdater(segment_vec, segment_size_field_index, segment_size, bits);
		
		bits = 32;
		uint32_t 
			exif_xres_offset = segment_size - exif_segment_xres_offset_size_diff,
			exif_yres_offset = segment_size - exif_segment_yres_offset_size_diff,
			exif_artist_size = (segment_size - exif_segment_size_diff) + 4, 
			exif_subifd_offset = segment_size - exif_segment_subifd_offset_size_diff;
			
		valueUpdater(segment_vec, exif_segment_xres_offset_field_index, exif_xres_offset, bits);
		valueUpdater(segment_vec, exif_segment_yres_offset_field_index, exif_yres_offset, bits);
		valueUpdater(segment_vec, exif_segment_artist_size_field_index, exif_artist_size, bits); 
		valueUpdater(segment_vec, exif_segment_subifd_offset_index, exif_subifd_offset, bits);
	} else {
		valueUpdater(segment_vec, segment_size_field_index, segment_size - PROFILE_MAIN_DIFF, bits);
		valueUpdater(segment_vec, profile_size_field_index, segment_size - PROFILE_INTERNAL_DIFF, bits);
	}
	image_vec.insert(image_vec.begin(), segment_vec.begin(), segment_vec.end());
	
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
