uint8_t imgPrmt(const std::string& IMAGE_FILENAME, const std::string& PROMPT_FILENAME, const std::string& URL_FILENAME, ArgOption platformOption) {

	const size_t
		IMAGE_FILE_SIZE = std::filesystem::file_size(IMAGE_FILENAME),
		PROMPT_FILE_SIZE = std::filesystem::file_size(PROMPT_FILENAME),
		URL_FILE_SIZE = std::filesystem::file_size(URL_FILENAME);


	if (!IMAGE_FILE_SIZE || !PROMPT_FILE_SIZE || !URL_FILE_SIZE) {
		std::cerr << "File Size Error: One or more of your files is empty!\n";
		return 1;
	}

	std::ifstream image_file_ifs(IMAGE_FILENAME, std::ios::binary);

	if (!image_file_ifs) {
		std::cerr << "\nRead File Error: Unable to read.\n\n";
		return 1;
	}

	std::vector<uint8_t> Image_Vec;
	Image_Vec.resize(IMAGE_FILE_SIZE);

	image_file_ifs.read(reinterpret_cast<char*>(Image_Vec.data()), IMAGE_FILE_SIZE);
	image_file_ifs.close();

	constexpr uint8_t
		SOI_SIG[]	{ 0xFF, 0xD8 },
		EOI_SIG[] 	{ 0xFF, 0xD9 };

	if (!std::equal(std::begin(SOI_SIG), std::end(SOI_SIG), std::begin(Image_Vec)) || !std::equal(std::begin(EOI_SIG), std::end(EOI_SIG), std::end(Image_Vec) - 2)) {
		std::cerr << "\nImage File Error: This is not a valid JPG image.\n\n";
		return 1;
	}
	
	const bool
		hasBlueSkyOption = (platformOption == ArgOption::BlueSky),
		hasDefaultOption = !hasBlueSkyOption;

	// For better compatibility, default re-encode image to JPG Progressive format with a quality value set at 97 with no chroma subsampling.
	// If Bluesky option, re-encode to standard Baseline format with a quality value set at 85.
	transcodeImage(image_vec, hasBlueskyOption);

	eraseSegments(Image_Vec);
	
	// This "locale" setting in Linux allows for the correct display and input (string variable storage) of wide characters.
	// Set the global C++ locale to en_US.UTF-8
	std::locale::global(std::locale("en_US.UTF-8"));

	// Update the locale settings according to the global locale
	std::cout.imbue(std::locale());

	// Open the saved user text files created from the imgprmt web form.
	std::ifstream url_file_ifs(URL_FILENAME, std::ios::binary);
	std::ifstream prompt_file_ifs(PROMPT_FILENAME, std::ios::binary);

	if (!url_file_ifs || !prompt_file_ifs) {
       		std::cerr << "Error opening text files (url or prompt)." << std::endl;
        	return 1;
	}

	std::vector<uint8_t>Prompt_Vec((std::istreambuf_iterator<char>(prompt_file_ifs)), std::istreambuf_iterator<char>());
	std::vector<uint8_t>Url_Vec((std::istreambuf_iterator<char>(url_file_ifs)), std::istreambuf_iterator<char>());

	url_file_ifs.close();
	prompt_file_ifs.close();

	constexpr uint16_t MAX_SEGMENT_SIZE = 10000;

	const uint16_t
		PROMPT_INSERT_INDEX = hasBlueSkyOption ? 0xE76 : 0xF3C,	// Insert location within Segment containing HTML page for the user's prompt text.
		LINK_INSERT_INDEX 	= hasBlueSkyOption ? 0xDE7 : 0xEAD;	// Insert location within Segment containing HTML page for the user's web link.

	// For X/Twitter, Mastodon, Tumblr, Flickr.
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


	if (hasDefaultOption) {
		Segment_Vec.insert(Segment_Vec.begin() + PROFILE_INSERT_INDEX, Profile_Vec.begin(), Profile_Vec.end());
	} else {
		Segment_Vec.swap(BlueSky_Vec);
	}

	// Insert prompt & the url link into their relevant index positions of the html section, within vector Segment_Vec.
	Segment_Vec.insert(Segment_Vec.begin() + PROMPT_INSERT_INDEX, Prompt_Vec.begin(), Prompt_Vec.end());
	Segment_Vec.insert(Segment_Vec.begin() + LINK_INSERT_INDEX, Url_Vec.begin(), Url_Vec.end());

	if (Segment_Vec.size() > MAX_SEGMENT_SIZE) {
		std::wcerr << L"\nFile Size Error: Data content size exceeds the maximum limit of 10KB.\n\n";
		return 1;
	}

	uint32_t segment_size = static_cast<uint32_t>(Segment_Vec.size());

	if (hasBlueSkyOption) {
		// For Bluesky segment size don't count/include the JPG ID + APP ID "FFD8FFE1" (4 bytes).
		segment_size -= 4;

		// Update EXIF segment size field (FFE1xxxx)
		valueUpdater(Segment_Vec, segment_size_field_index, segment_size, bits);

		bits = 32;
		uint32_t
			exif_xres_offset = segment_size - exif_segment_xres_offset_size_diff,
			exif_yres_offset = segment_size - exif_segment_yres_offset_size_diff,
			exif_artist_size = (segment_size - exif_segment_size_diff) + 4, // For this variable, include the JPG ID + APP ID "FFD8FFE1" (4 bytes).
			exif_subifd_offset = segment_size - exif_segment_subifd_offset_size_diff;

		valueUpdater(Segment_Vec, exif_segment_xres_offset_field_index, exif_xres_offset, bits);
		valueUpdater(Segment_Vec, exif_segment_yres_offset_field_index, exif_yres_offset, bits);
		valueUpdater(Segment_Vec, exif_segment_artist_size_field_index, exif_artist_size, bits);
		valueUpdater(Segment_Vec, exif_segment_subifd_offset_index, exif_subifd_offset, bits);
	} else {
		// Update color profile segment size field (FFE2xxxx)
		valueUpdater(Segment_Vec, segment_size_field_index, segment_size - PROFILE_MAIN_DIFF, bits);

		// Update internal color profile size field
		valueUpdater(Segment_Vec, profile_size_field_index, segment_size - PROFILE_INTERNAL_DIFF, bits);
	}

	Image_Vec.insert(Image_Vec.begin(), Segment_Vec.begin(), Segment_Vec.end());

	// Clear
	std::vector<uint8_t>().swap(Segment_Vec);

	const uint32_t IMAGE_SIZE = static_cast<uint32_t>(Image_Vec.size());

	srand((unsigned)time(NULL));  
        const std::string 
		TIME_VALUE = std::to_string(rand()),
		OUTPUT_FILENAME = "imgprmt_" + TIME_VALUE.substr(0, 5) + ".jpg"; 

	std::ofstream write_file_fs(OUTPUT_FILENAME, std::ios::binary);
	if (!write_file_fs) {
		std::cerr << "\nWrite Error: Unable to write to file.\n\n";
		return 1;
	}

	write_file_fs.write((char*)&Image_Vec[0], IMAGE_SIZE);
	std::vector<uint8_t>().swap(Image_Vec);
	std::cout << "\nCreated output file: " + OUTPUT_FILENAME + " " << IMAGE_SIZE << " " << "Bytes\n\n";
	return 0;
}
