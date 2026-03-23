//  imgprmt v1.3 (Linux CLI Edition). Created by Nicholas Cleasby (@CleasbyCode) 19/05/2023

// Compile program (Linux):

// $ sudo apt install libturbojpeg0-dev

// $ chmod +x compile_imgprmt.sh
// $ ./compile_imgprmt.sh

// $ Compilation successful. Executable 'imgprmt' created.
// $ sudo cp imgprmt /usr/bin
// $ imgprmt

#include "imgprmt_app.hpp"
#include "imgprmt_args.hpp"
#include "imgprmt_cli.hpp"
#include "imgprmt_io.hpp"
#include "imgprmt_pipeline.hpp"
#include "imgprmt_template.hpp"
#include "imgprmt_text.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
	using Byte   = std::uint8_t;
	using vBytes = std::vector<Byte>;
}

namespace imgprmt {
int run_cli(int argc, char** argv) {
		try {
			auto args_opt = program_args::parse(argc, argv);
	        if (!args_opt) return 0;

			program_args args = *args_opt;
			io::apply_runtime_limits();

		bool isBluesky = (args.option == Option::Bluesky);
		pipeline::prepared_cover cover = pipeline::prepare_cover_image(args.image_file_path, isBluesky);

		cli::prompt_data prompt = cli::read_prompt_data(isBluesky);

		const auto mode = (args.option == Option::Bluesky)
		    ? polyglot::SegmentMode::Bluesky
		    : polyglot::SegmentMode::Default;
		vBytes segment_vec = polyglot::build_segment(mode, std::move(prompt.utf8_prompt), std::move(prompt.utf8_url));

		pipeline::output_result output = pipeline::write_output_image(
			std::span<const Byte>(segment_vec.data(), segment_vec.size()),
			std::span<const Byte>(cover.jpg_vec.data(), cover.jpg_vec.size()));

		vBytes().swap(segment_vec);
		vBytes().swap(cover.jpg_vec);
		std::println("\nSaved \"prompt-embedded\" JPG image: {} ({} bytes).\n", output.path, output.total_bytes);
		return 0;
	}
	catch (const std::exception& e) {
    	std::println(std::cerr, "\n{}\n", e.what());
    	return 1;
    }
}

}  // namespace imgprmt
