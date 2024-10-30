int imgPrmt(const std::string& IMAGE_FILENAME) {

	constexpr uint32_t MAX_FILE_SIZE = 20971520; // 20MB.

	const size_t TMP_IMAGE_FILE_SIZE = std::filesystem::file_size(IMAGE_FILENAME);
	
	std::ifstream image_ifs(IMAGE_FILENAME, std::ios::binary);

	if (!image_ifs || TMP_IMAGE_FILE_SIZE > MAX_FILE_SIZE) {
		std::cerr << (!image_ifs 
			? "\nOpen File Error: Unable to read image file"
			: "\nImage File Error: Size of file exceeds the maximum 20MB limit for this program")
		<< ".\n\n";
		return 1;
	}

	std::vector<uint8_t> Image_Vec;
	Image_Vec.reserve(TMP_IMAGE_FILE_SIZE);

	std::copy(std::istreambuf_iterator<char>(image_ifs), std::istreambuf_iterator<char>(), std::back_inserter(Image_Vec));	

	constexpr uint8_t
		SOI_SIG[]	{ 0xFF, 0xD8 },
		EOI_SIG[] 	{ 0xFF, 0xD9 };

	if (!std::equal(std::begin(SOI_SIG), std::end(SOI_SIG), std::begin(Image_Vec)) || !std::equal(std::begin(EOI_SIG), std::end(EOI_SIG), std::end(Image_Vec) - 2)) {
        	std::cerr << "\nImage File Error: This is not a valid JPG image.\n\n";
		return 1;
	}
	
	bool isKdakProfile = false;

	eraseSegments(Image_Vec, isKdakProfile);
	
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

	constexpr uint16_t
		MAX_PROFILE_SIZE 	= 10000,	// X/Twitter ICC Profile size limit.	
		PROMPT_INSERT_INDEX 	= 0xF3C,	// (Default Profile) Insert location within Profile_Vec of the HTML page for the users's prompt text.
		LINK_INSERT_INDEX 	= 0xEAD,	// (Default Profile) Insert location within Profile_Vec of the HTML page for the user's web link.
		KDAK_PROFILE_SIZE_DIFF 	= 60,		// Kdak Profile is 60 bytes greater than the Default Profile.
		KDAK_PROMPT_INSERT_INDEX = PROMPT_INSERT_INDEX + KDAK_PROFILE_SIZE_DIFF, // (Kdak Profile) Insert location within Profile_Vec of the HTML page for the users's prompt text.
		KDAK_LINK_INSERT_INDEX 	= LINK_INSERT_INDEX + KDAK_PROFILE_SIZE_DIFF;	 // (Kdak Profile) Insert location within Profile_Vec of the HTML page for the user's web link (url).

	constexpr uint8_t
		PROFILE_INTERNAL_DIFF 	= 38,	// Bytes we don't count as part of internal profile size.
		PROFILE_MAIN_DIFF 	= 22,	// Bytes we don't count as part of profile size.
		PROFILE_INSERT_INDEX 	= 0x14;	// Insert location within Profile_Vec for the ICC Profile (Default or Kdak).

	uint8_t
		profile_internal_size_field_index = 0x28,	// Start index location for internal size field of the image icc profile.(Max four bytes, only two used).
		profile_size_field_index = 0x16,		// Start index location for size field of the main image icc profile. (Max two bytes)
		bits = 16;
		
	isKdakProfile 
		? Profile_Vec.insert(Profile_Vec.begin() + PROFILE_INSERT_INDEX, Kdak_Profile_Vec.begin(), Kdak_Profile_Vec.end()) 
		: Profile_Vec.insert(Profile_Vec.begin() + PROFILE_INSERT_INDEX, Default_Profile_Vec.begin(), Default_Profile_Vec.end());
	
	std::vector<uint8_t>().swap(Kdak_Profile_Vec);
	std::vector<uint8_t>().swap(Default_Profile_Vec);

	// Insert image prompt/description & url link into their relevant index positions within vector "Profile_Vec".
	Profile_Vec.insert(Profile_Vec.begin() + (isKdakProfile ? KDAK_PROMPT_INSERT_INDEX : PROMPT_INSERT_INDEX), multiByteString, multiByteString + bufferSize - 1);
	Profile_Vec.insert(Profile_Vec.begin() + (isKdakProfile ? KDAK_LINK_INSERT_INDEX : LINK_INSERT_INDEX), url_link.begin(), url_link.end());

	delete[] multiByteString;

	if (Profile_Vec.size() > MAX_PROFILE_SIZE) {
		std::wcerr << L"\nFile Size Error: Data content size exceeds the maximum limit of 10KB.\n\n";
		return 1;
	}

	const uint32_t PROFILE_SIZE = static_cast<uint32_t>(Profile_Vec.size());

	// Update main profile size 
	valueUpdater(Profile_Vec, profile_size_field_index, PROFILE_SIZE - PROFILE_MAIN_DIFF, bits);

	// Update internal profile size
	valueUpdater(Profile_Vec, profile_internal_size_field_index, PROFILE_SIZE - PROFILE_INTERNAL_DIFF, bits);

	Image_Vec.insert(Image_Vec.begin(), Profile_Vec.begin(), Profile_Vec.end());
	const uint32_t IMAGE_SIZE = static_cast<uint32_t>(Image_Vec.size());

	std::vector<uint8_t>().swap(Profile_Vec);

	srand((unsigned)time(NULL)); 

	const std::wstring 
		TIME_VALUE = std::to_wstring(rand()),
		OUTPUT_FILENAME = L"imgprmt_" + TIME_VALUE.substr(0, 5) + L".jpg";

	std::ofstream file_ofs(OUTPUT_FILENAME, std::ios::binary);

	if (!file_ofs) {
		std::wcerr << L"\nWrite Error: Unable to write to file.\n\n";
		return 1;
	}
	
	file_ofs.write((char*)&Image_Vec[0], IMAGE_SIZE);
	std::vector<uint8_t>().swap(Image_Vec);
	std::wcout << L"\nCreated output file: \"" + OUTPUT_FILENAME + L" " << IMAGE_SIZE << " " << "Bytes\"\n\n";
        return 0;
}
