uint_fast8_t imgPrmt(const std::string& IMAGE_FILENAME) {

	constexpr uint_fast32_t MAX_FILE_SIZE = 20971520; // 20MB.

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

	constexpr uint_fast8_t
		SOI_SIG[]	{ 0xFF, 0xD8 },
		EOI_SIG[] 	{ 0xFF, 0xD9 };

	if (!std::equal(std::begin(SOI_SIG), std::end(SOI_SIG), std::begin(Image_Vec)) 
		|| !std::equal(std::begin(EOI_SIG), std::end(EOI_SIG), std::end(Image_Vec) - 2)) {
        		std::cerr << "\nImage File Error: This is not a valid JPG image.\n\n";
		return 1;
	}
	
	bool isKdakProfile = false;

	eraseSegments(Image_Vec, isKdakProfile);
	
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
        	return 1;
        }

	const size_t TMP_PROMPT_SIZE = std::filesystem::file_size("/var/www/cleasbycode.co.uk/html/imgprmt/uploads/prompt.txt");

	if (!TMP_PROMPT_SIZE) {
		std::cerr << "File Size Error: Prompt text file is empty!\n";
		return 1;
	}

	std::vector<uint8_t>Prompt_Vec((std::istreambuf_iterator<char>(prompt_file_ifs)), std::istreambuf_iterator<char>());
	std::vector<uint8_t>Url_Vec((std::istreambuf_iterator<char>(url_file_ifs)), std::istreambuf_iterator<char>());

 	constexpr uint_fast16_t
		MAX_PROFILE_SIZE = 10000,	// X/Twitter ICC Profile size limit.	
		PROMPT_INSERT_INDEX = 0xF3C,	// (Default Profile) Insert location within Profile_Vec of the HTML page for the users's prompt text.
		LINK_INSERT_INDEX = 0xEAD,	// (Default Profile) Insert location within Profile_Vec of the HTML page for the user's web link.
		KDAK_PROFILE_SIZE_DIFF = 60,	// Kdak Profile is 60 bytes greater than the Default Profile.
		KDAK_PROMPT_INSERT_INDEX = PROMPT_INSERT_INDEX + KDAK_PROFILE_SIZE_DIFF, // (Kdak Profile) Insert location within Profile_Vec of the HTML page for the users's prompt text.
		KDAK_LINK_INSERT_INDEX = LINK_INSERT_INDEX + KDAK_PROFILE_SIZE_DIFF;	 // (Kdak Profile) Insert location within Profile_Vec of the HTML page for the user's web link (url).

	constexpr uint_fast8_t
		PROFILE_INTERNAL_DIFF = 38,	// Bytes we don't count as part of internal profile size.
		PROFILE_MAIN_DIFF = 22,		// Bytes we don't count as part of profile size.
		PROFILE_INSERT_INDEX = 0x14;	// Insert location within Profile_Vec for the ICC Profile (Default or Kdak).

	uint_fast8_t
		profile_internal_size_field_index = 0x28,	// Start index location for internal size field of the image icc profile.(Max four bytes, only two used).
		profile_size_field_index = 0x16,		// Start index location for size field of the main image icc profile. (Max two bytes)
		bits = 16;
		
	isKdakProfile 
		? Profile_Vec.insert(Profile_Vec.begin() + PROFILE_INSERT_INDEX, Kdak_Profile_Vec.begin(), Kdak_Profile_Vec.end()) 
		: Profile_Vec.insert(Profile_Vec.begin() + PROFILE_INSERT_INDEX, Default_Profile_Vec.begin(), Default_Profile_Vec.end());
	
	std::vector<uint8_t>().swap(Kdat_Profile_Vec);
	std::vector<uint8_t>().swap(Default_Profile_Vec);
	
	// Insert image prompt & URL link into their relevant index positions within vector Profile_Vec.
	Profile_Vec.insert(Profile_Vec.begin() + (isKdakProfile ? PROMPT_INSERT_INDEX + KDAK_PROFILE_SIZE_DIFF : PROMPT_INSERT_INDEX), Prompt_Vec.begin(), Prompt_Vec.end());
	Profile_Vec.insert(Profile_Vec.begin() + (isKdakProfile ? LINK_INSERT_INDEX + KDAK_PROFILE_SIZE_DIFF : LINK_INSERT_INDEX), Url_Vec.begin(), Url_Vec.end());

	const uint_fast32_t PROFILE_VEC_SIZE = static_cast<uint_fast32_t>(Profile_Vec.size());
					 
	if (PROFILE_VEC_SIZE > MAX_PROFILE_SIZE) {
		std::cerr << "\nFile Size Error: Data content size exceeds the maximum limit of 10KB.\n\n";
		return 1;
	}

	// Update main profile size & internal profile length fields
	valueUpdater(Profile_Vec, profile_size_field_index, PROFILE_VEC_SIZE - PROFILE_MAIN_DIFF, bits);
	valueUpdater(Profile_Vec, profile_internal_size_field_index, PROFILE_VEC_SIZE - PROFILE_INTERNAL_DIFF, bits);

	Image_Vec.insert(Image_Vec.begin(), Profile_Vec.begin(), Profile_Vec.end());
	const uint_fast32_t IMAGE_VEC_SIZE = static_cast<uint_fast32_t>(Image_Vec.size());
					 
	std::vector<uint8_t>().swap(Profile_Vec);

	srand((unsigned)time(NULL));  
        const std::string 
		TIME_VALUE = std::to_string(rand()),
		OUTPUT_FILENAME = "imgprmt_" + TIME_VALUE.substr(0, 5) + ".jpg"; 

	std::ofstream write_file_fs(OUTPUT_FILENAME, std::ios::binary);
	if (!write_file_fs) {
		std::cerr << "\nWrite Error: Unable to write to file.\n\n";
		return 1;
	}

	write_file_fs.write((char*)&Image_Vec[0], IMAGE_VEC_SIZE);
	std::vector<uint8_t>().swap(Image_Vec);
	std::cout << "\nCreated output file: " + OUTPUT_FILENAME + " " << IMAGE_VEC_SIZE << " " << "Bytes\n\n";
	return 0;
}
