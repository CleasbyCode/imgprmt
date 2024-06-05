void startImgprmt(const std::string& IMAGE_FILENAME) {

	std::ifstream image_file_ifs(IMAGE_FILENAME, std::ios::binary);

	if (!image_file_ifs) {
		std::cerr << "\nRead Error: Unable to open/read file: \"" + IMAGE_FILENAME + "\"\n\n";
		std::exit(EXIT_FAILURE);
	}

	std::vector<uint_fast8_t>Image_Vec((std::istreambuf_iterator<char>(image_file_ifs)), std::istreambuf_iterator<char>());

	constexpr uint_fast8_t
		JPG_SIG[]		{ 0xFF, 0xD8, 0xFF },
		JPG_END_SIG[]	{ 0xFF, 0xD9 },
		DQT_SIG[]		{ 0xFF, 0xDB },
		EXIF_SIG[]		{ 0x45, 0x78, 0x69, 0x66, 0x00, 0x00, 0x49, 0x49 },
		XPACKET_SIG[]	{ 0x78, 0x70, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x20, 0x65, 0x6E, 0x64 },
		ICC_KODAK_SIG[]	{ 0x4B, 0x4F, 0x44, 0x41, 0x52, 0x4F, 0x4D },
		ICC_PROFILE_SIG[]{ 0x49, 0x43, 0x43, 0x5F, 0x50, 0x52, 0x4F, 0x46, 0x49, 0x4C, 0x45 };
	
	if (!std::equal(std::begin(JPG_SIG), std::end(JPG_SIG), std::begin(Image_Vec)) || !std::equal(std::begin(JPG_END_SIG), std::end(JPG_END_SIG), std::end(Image_Vec) - 2)) {
		std::cerr << "\nImage File Error: This is not a valid JPG image.\n\n";
		std::exit(EXIT_FAILURE);
	}

	bool isKodak_Profile = false;

	const uint_fast32_t ICC_PROFILE_POS = searchFunc(Image_Vec, 0, 0, ICC_PROFILE_SIG);

	if (Image_Vec.size() > ICC_PROFILE_POS) {
		
		const uint_fast32_t ICC_KODAK_POS = searchFunc(Image_Vec, ICC_PROFILE_POS, 0, ICC_KODAK_SIG);
		if (Image_Vec.size() > ICC_KODAK_POS) {
			isKodak_Profile = true;	// If an image has this profile, we need to put it back later, so as to retain image color quality.
		}

		Image_Vec.erase(Image_Vec.begin(), Image_Vec.begin() + ICC_PROFILE_POS);
	}

	const uint_fast32_t XPACKET_SIG_POS = searchFunc(Image_Vec, 0, 0, XPACKET_SIG);

	if (Image_Vec.size() > XPACKET_SIG_POS) {
		Image_Vec.erase(Image_Vec.begin(), Image_Vec.begin() + (XPACKET_SIG_POS + 0x11));
	}

	const uint_fast32_t EXIF_POS = searchFunc(Image_Vec, 0, 0, EXIF_SIG);

	if (Image_Vec.size() > EXIF_POS) {
		const uint_fast16_t EXIF_BLOCK_SIZE = (static_cast<uint_fast16_t>(Image_Vec[EXIF_POS - 2]) << 8) | static_cast<uint_fast16_t>(Image_Vec[EXIF_POS - 1]);
		Image_Vec.erase(Image_Vec.begin(), Image_Vec.begin() + EXIF_BLOCK_SIZE - 2);
	}
	
	const uint_fast32_t DQT_POS = searchFunc(Image_Vec, 0, 0, DQT_SIG);

	Image_Vec.erase(Image_Vec.begin(), Image_Vec.begin() + DQT_POS);

	std::string
		prompt,
		url_link;

	std::cout << "\n\nEnter a Web link (Image source, Social media page, etc.)\n\nURL: ";
	std::getline(std::cin, url_link);

	fflush(stdin); fflush(stdout);

	std::ignore = _setmode(_fileno(stdin), 0x20000);  // Set standard input for Windows to "_O_U16TEXT" so our wide variables contain correct wide characters.
	std::ignore = _setmode(_fileno(stdout), 0x20000); // Set standard output for Windows to "_O_U16TEXT" so the console displays correct wide characters.

	std::wcout << L"\nType or paste in your prompt as one long sentence. Add <br> tags for new lines, if required.\n\nImage Description: ";

	constexpr uint_fast32_t wcin_buffer_size = 8000;  	// The default string length in Windows console for "wcin" when "_O_U16TEXT" is set, 
														// seems to be just over 2000 characters. So we increase it here to 8000 to provide a more flexible size.

	wchar_t* wcin_buffer = new wchar_t[wcin_buffer_size]();

	std::wcin.rdbuf()->pubsetbuf(wcin_buffer, wcin_buffer_size); // Set the new buffer size, as above.

	std::wcin.getline(wcin_buffer, wcin_buffer_size);
	std::wstring wide_prompt(wcin_buffer);

	// Search the wide string for problem characters and replace them with html entity codes.
	replaceProblemChars(wide_prompt);

	// Determine the required buffer size for the multibyte string
	uint_fast32_t bufferSize = WideCharToMultiByte(CP_UTF8, 0, wide_prompt.c_str(), -1, nullptr, 0, nullptr, nullptr);

	if (bufferSize == 0) {
		std::wcerr << L"Error: Unable to determine buffer size\n";
		std::exit(EXIT_FAILURE);
	}

	// Allocate memory for the multibyte string
	char* multiByteString = new char[bufferSize];

	// Convert the wide character string to multibyte (UTF-8) string
	if (WideCharToMultiByte(CP_UTF8, 0, wide_prompt.c_str(), -1, multiByteString, bufferSize, nullptr, nullptr) == 0) {
		std::wcerr << L"Error: Conversion failed\n";
		delete[] multiByteString;
		std::exit(EXIT_FAILURE);
	}

	delete[] wcin_buffer;

	constexpr uint_fast16_t
		MAX_PROFILE_SIZE = 10000,		
		PROMPT_INSERT_INDEX = 3900,		// (Default Profile) Insert location within Profile_Vec of the HTML page for the users's prompt text.
		LINK_INSERT_INDEX = 3757,		// (Default Profile) Insert location within Profile_Vec of the HTML page for the user's web link.
		KODAK_PROFILE_SIZE_DIFF = 775,	// Kodak Profile is 775 bytes greater than the Default Profile.
		KODAK_PROMPT_INSERT_INDEX = PROMPT_INSERT_INDEX + KODAK_PROFILE_SIZE_DIFF, // (Kodak Profile) Insert location within Profile_Vec of the HTML page for the users's prompt text.
		KODAK_LINK_INSERT_INDEX = LINK_INSERT_INDEX + KODAK_PROFILE_SIZE_DIFF;	   // (Kodak Profile) Insert location within Profile_Vec of the HTML page for the user's web link (url).

	constexpr uint_fast8_t
		PROFILE_INTERNAL_DIFF = 38,	// Bytes we don't count as part of internal profile size.
		PROFILE_MAIN_DIFF = 22,		// Bytes we don't count as part of profile size.
		PROFILE_INSERT_INDEX = 20;	// Insert location within Profile_Vec for the ICC Profile (Default or Kodak).

	uint_fast8_t
		profile_internal_size_field = 40,	// Start index location for internal size field of the image icc profile.(Max four bytes, only two used).
		profile_size_field = 22,			// Start index location for size field of the main image icc profile. (Max two bytes)
		bits = 16;
		
	isKodak_Profile 
		? Profile_Vec.insert(Profile_Vec.begin() + PROFILE_INSERT_INDEX, Kodak_Profile_Vec.begin(), Kodak_Profile_Vec.end()) 
		: Profile_Vec.insert(Profile_Vec.begin() + PROFILE_INSERT_INDEX, Default_Profile_Vec.begin(), Default_Profile_Vec.end());
	

	Kodak_Profile_Vec.clear();
	Default_Profile_Vec.clear();

	Kodak_Profile_Vec.shrink_to_fit();
	Default_Profile_Vec.shrink_to_fit();

	// Insert image prompt/description & url link into their relevant index positions within vector "Profile_Vec".
	Profile_Vec.insert(Profile_Vec.begin() + (isKodak_Profile ? PROMPT_INSERT_INDEX + KODAK_PROFILE_SIZE_DIFF : PROMPT_INSERT_INDEX), multiByteString, multiByteString + bufferSize - 1);
	Profile_Vec.insert(Profile_Vec.begin() + (isKodak_Profile ? LINK_INSERT_INDEX + KODAK_PROFILE_SIZE_DIFF : LINK_INSERT_INDEX), url_link.begin(), url_link.end());

	delete[] multiByteString;

	if (Profile_Vec.size() > MAX_PROFILE_SIZE) {
		std::wcerr << L"\nFile Size Error: Data content size exceeds the maximum limit of 10KB.\n\n";
		std::exit(EXIT_FAILURE);
	}

	// Update main profile size 
	valueUpdater(Profile_Vec, profile_size_field, static_cast<uint_fast32_t>(Profile_Vec.size()) - PROFILE_MAIN_DIFF, bits);

	// Update internal profile size
	valueUpdater(Profile_Vec, profile_internal_size_field, static_cast<uint_fast32_t>(Profile_Vec.size()) - PROFILE_INTERNAL_DIFF, bits);

	Image_Vec.insert(Image_Vec.begin(), Profile_Vec.begin(), Profile_Vec.end());

	Profile_Vec.clear();
	Profile_Vec.shrink_to_fit();

	srand((unsigned)time(NULL)); 

	const std::wstring 
		TIME_VALUE = std::to_wstring(rand()),
		OUTPUT_FILENAME = L"imgprmt_" + TIME_VALUE.substr(0, 5) + L".jpg";

	std::ofstream file_ofs(OUTPUT_FILENAME, std::ios::binary);

	if (!file_ofs) {
		std::wcerr << L"\nWrite Error: Unable to write to file.\n\n";
		std::exit(EXIT_FAILURE);
	}

	file_ofs.write((char*)&Image_Vec[0], Image_Vec.size());
	std::wcout << L"\nCreated output file: \"" + OUTPUT_FILENAME + L" " << Image_Vec.size() << " " << "Bytes\"\n\n";
}
