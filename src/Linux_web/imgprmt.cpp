void startImgprmt(const std::string& IMAGE_FILENAME) {

	std::ifstream image_file_ifs(IMAGE_FILENAME, std::ios::binary);

	if (!image_file_ifs) {
		std::cerr << "\nRead Error: Unable to open/read file: \"" + IMAGE_FILENAME + "\"\n\n";
		std::exit(EXIT_FAILURE);
	}

	std::vector<uint_fast8_t>Image_Vec((std::istreambuf_iterator<char>(image_file_ifs)), std::istreambuf_iterator<char>());

	constexpr uint_fast8_t
		JPG_SIG[]		{ 0xFF, 0xD8, 0xFF },
		JPG_END_SIG[]		{ 0xFF, 0xD9 },
		DQT_SIG[]		{ 0xFF, 0xDB },
		EXIF_SIG[]		{ 0x45, 0x78, 0x69, 0x66, 0x00, 0x00, 0x49, 0x49 },
		XPACKET_SIG[]		{ 0x78, 0x70, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x20, 0x65, 0x6E, 0x64 },
		ICC_KODAK_SIG[]		{ 0x4B, 0x4F, 0x44, 0x41, 0x52, 0x4F, 0x4D },
		ICC_PROFILE_SIG[]	{ 0x49, 0x43, 0x43, 0x5F, 0x50, 0x52, 0x4F, 0x46, 0x49, 0x4C, 0x45 };
	
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
	
	// This "locale" setting in Linux allows for the correct display and input (string variable storage) of wide characters.
	// Set the global C++ locale to en_US.UTF-8
	std::locale::global(std::locale("en_US.UTF-8"));

    	// Update the locale settings according to the global locale
    	std::cout.imbue(std::locale());

	// Open the saved user text files created from the imgprmt web form.
	std::ifstream url_file_ifs("/var/www/cleasbycode.co.uk/html/imgprmt/uploads/url.txt", std::ios::binary);
	std::ifstream prompt_file_ifs("/var/www/cleasbycode.co.uk/html/imgprmt/uploads/prompt.txt", std::ios::binary);

       	if (!url_file_ifs || !prompt_file_ifs) {
       		std::cerr << "Error opening text files (url or prompt)." << std::endl;
        	std::exit(EXIT_FAILURE);
        }

	const size_t TMP_PROMPT_SIZE = std::filesystem::file_size("/var/www/cleasbycode.co.uk/html/imgprmt/uploads/prompt.txt");

	if (!TMP_PROMPT_SIZE) {
		std::cerr << "File Size Error: Prompt text file is empty!\n";
		std::exit(EXIT_FAILURE);
	}

	std::vector<uint_fast8_t>Prompt_Vec((std::istreambuf_iterator<char>(prompt_file_ifs)), std::istreambuf_iterator<char>());
	std::vector<uint_fast8_t>Url_Vec((std::istreambuf_iterator<char>(url_file_ifs)), std::istreambuf_iterator<char>());

 constexpr uint_fast16_t
		MAX_PROFILE_SIZE = 10000,	// X(Twitter) size limit ~10KB.	
		PROMPT_INSERT_INDEX = 3900,	// (Default Profile) Insert location within Profile_Vec of the HTML page for the users's prompt text.
		LINK_INSERT_INDEX = 3757,	// (Default Profile) Insert location within Profile_Vec of the HTML page for the user's web link.
		KODAK_PROFILE_SIZE_DIFF = 775,	// Kodak Profile is 775 bytes greater than the Default Profile.
		KODAK_PROMPT_INSERT_INDEX = PROMPT_INSERT_INDEX + KODAK_PROFILE_SIZE_DIFF, // (Kodak Profile) Insert location within Profile_Vec of the HTML page for the users's prompt text.
		KODAK_LINK_INSERT_INDEX = LINK_INSERT_INDEX + KODAK_PROFILE_SIZE_DIFF;	   // (Kodak Profile) Insert location within Profile_Vec of the HTML page for the user's web link (url).

	constexpr uint_fast8_t
		PROFILE_INTERNAL_DIFF = 38,	// Bytes we don't count as part of internal profile size.
		PROFILE_MAIN_DIFF = 22,		// Bytes we don't count as part of profile size.
		PROFILE_INSERT_INDEX = 20;	// Insert location within Profile_Vec for the ICC Profile (Default or Kodak).

	uint_fast8_t
		profile_internal_size_field = 40,	// Start index location for internal size field of the image icc profile.(Max four bytes, only two used).
		profile_size_field = 22,		// Start index location for size field of the main image icc profile. (Max two bytes)
		bits = 16;
		
	isKodak_Profile 
		? Profile_Vec.insert(Profile_Vec.begin() + PROFILE_INSERT_INDEX, Kodak_Profile_Vec.begin(), Kodak_Profile_Vec.end()) 
		: Profile_Vec.insert(Profile_Vec.begin() + PROFILE_INSERT_INDEX, Default_Profile_Vec.begin(), Default_Profile_Vec.end());
					 
	// Clean-up, no longer needed.
	Kodak_Profile_Vec.clear();	
	Default_Profile_Vec.clear();
					 
	Kodak_Profile_Vec.shrink_to_fit();
	Default_Profile_Vec.shrink_to_fit();

	// Insert image prompt & URL link into their relevant index positions within vector Profile_Vec.
	Profile_Vec.insert(Profile_Vec.begin() + (isKodak_Profile ? PROMPT_INSERT_INDEX + KODAK_PROFILE_SIZE_DIFF : PROMPT_INSERT_INDEX), Prompt_Vec.begin(), Prompt_Vec.end());
	Profile_Vec.insert(Profile_Vec.begin() + (isKodak_Profile ? LINK_INSERT_INDEX + KODAK_PROFILE_SIZE_DIFF : LINK_INSERT_INDEX), Url_Vec.begin(), Url_Vec.end());

	const uint_fast32_t PROFILE_VEC_SIZE = static_cast<uint_fast32_t>(Profile_Vec.size());
					 
	if (PROFILE_VEC_SIZE > MAX_PROFILE_SIZE) {
		std::cerr << "\nFile Size Error: Data content size exceeds the maximum limit of 10KB.\n\n";
		std::exit(EXIT_FAILURE);
	}

	// Update main profile size & internal profile length fields
	valueUpdater(Profile_Vec, profile_size_field, PROFILE_VEC_SIZE - PROFILE_MAIN_DIFF, bits);
	valueUpdater(Profile_Vec, profile_internal_size_field, PROFILE_VEC_SIZE - PROFILE_INTERNAL_DIFF, bits);

	Image_Vec.insert(Image_Vec.begin(), Profile_Vec.begin(), Profile_Vec.end());
	const uint_fast32_t IMAGE_VEC_SIZE = static_cast<uint_fast32_t>(Image_Vec.size());
					 
	Profile_Vec.clear();
	Profile_Vec.shrink_to_fit();

	srand((unsigned)time(NULL));  
        const std::string 
		TIME_VALUE = std::to_string(rand()),
		OUTPUT_FILENAME = "imgprmt_" + TIME_VALUE.substr(0, 5) + ".jpg"; 

	std::ofstream write_file_fs(OUTPUT_FILENAME, std::ios::binary);

	if (!write_file_fs) {
		std::cerr << "\nWrite Error: Unable to write to file.\n\n";
		std::exit(EXIT_FAILURE);
	}

	write_file_fs.write((char*)&Image_Vec[0], IMAGE_VEC_SIZE);
	std::cout << "\nCreated output file: " + OUTPUT_FILENAME + " " << IMAGE_VEC_SIZE << " " << "Bytes\n\n";
}

