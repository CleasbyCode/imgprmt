#include "imgprmt_app.hpp"
#include "imgprmt_jpeg.hpp"
#include "imgprmt_text.hpp"
#include "imgprmt_url.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <string>
#include <vector>

namespace imgprmt {
int fuzz_url_input(const std::uint8_t* data, std::size_t size) {
	if (!data || size == 0) {
		return 0;
	}

	using Byte = std::uint8_t;
	using vBytes = std::vector<Byte>;

	constexpr std::size_t MAX_FUZZ_INPUT = 4096;
	const std::size_t clipped = std::min(size, MAX_FUZZ_INPUT);

	std::string ascii_in(reinterpret_cast<const char*>(data), clipped);
	(void)url::validate_url_link(ascii_in);
	(void)url::valid_path_query_ascii(ascii_in);

	vBytes bytes(data, data + clipped);
	constexpr auto DQT_SIG = std::to_array<Byte>({0xFF, 0xDB});
	(void)std::ranges::search(bytes, DQT_SIG);
	(void)jpeg::exifOrientation(bytes);
	(void)jpeg::estimateImageQuality(bytes);

	if (!ascii_in.empty()) {
		std::wstring w;
		w.reserve(std::min<std::size_t>(ascii_in.size(), 1024));
		for (char c : ascii_in) {
			w.push_back(static_cast<wchar_t>(static_cast<unsigned char>(c)));
		}
		text::replace_problem_chars(w);
	}

	return 0;
}
}  // namespace imgprmt
