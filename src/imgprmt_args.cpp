#include "imgprmt_args.hpp"

#include <format>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>

namespace fs = std::filesystem;

namespace {
void display_info() {
	std::print(R"(

Imgprmt v1.3 (Linux CLI Edition).
Created by Nicholas Cleasby (@CleasbyCode) 25/05/2023.

imgprmt is a command-line tool for Linux that
embeds image text prompts into JPG images.

It outputs a unique JPG-HTML polyglot file, combining image, prompt text & html.

Share the resulting image on compatible social media platforms like:-

X-Twitter, Tumblr, Mastodon, Pixelfed, Flickr & *Bluesky.

The embedded prompt is preserved for anyone who downloads the image.

To view the stored prompt locally, rename the image extension to ".htm" and open
it in a web browser to display the basic webpage.

For a convenient alternative to downloading & compiling the CLI source code, use the imgprmt Web App:-

https://cleasbycode.co.uk/imgprmt/app/
________________________
Compile & run (Linux)
________________________

  $ sudo apt-get install libturbojpeg0-dev

  $ chmod +x compile_imgprmt.sh
  $ ./compile_imgprmt.sh

  Compilation successful. Executable 'imgprmt' created.

  $ sudo cp imgprmt /usr/bin
  $ imgprmt
________________________
Usage
________________________

  imgprmt [-b] <jpg_image>
  imgprmt --info

With the default command-line arguments without any option(s),
the embedded image can be posted on X-Twitter, Tumblr, Mastodon Pixelfed & Flickr.

The default prompt limit is 57140 bytes (minus URL address length).

X-Twitter, for compatibility reasons, is limited to 1845 bytes (minus URL address length).
Considerably smaller than the default size, but should be adequate for most prompts.

If you intend to share your prompt-embedded image on Bluesky,
you must first add the -b option to the command-line arguments:-

$ imgprmt -b my_image.jpg

Secondly, to post the image on Bluesky, you will need to use the bsky_post.py script,
located in the repo src folder.

To run this script, you need Python3 and the 'requests' and 'bs4' (BeautifulSoup)
Python packages installed.

An app-password is also required for the Python script, which you can create
from your Bluesky account: https://bsky.app/settings/app-passwords

Here are some basic usage examples for the bsky_post.py Python script:

Standard image post to your profile/account.

$ python3 bsky_post.py --handle you.bsky.social --password xxxx-xxxx-xxxx-xxxx
--image your_image.jpg --alt-text "alt-text here [optional]" "standard post text here [required]"

If you want to post multiple images (Max. 4):

$ python3 bsky_post.py --handle you.bsky.social --password xxxx-xxxx-xxxx-xxxx
--image img1.jpg --image img2.jpg --alt-text "alt_here" "standard post text..."

If you want to post an image as a reply to another thread:

$ python3 bsky_post.py --handle you.bsky.social --password xxxx-xxxx-xxxx-xxxx
--image your_image.jpg --alt-text "alt_here"
--reply-to https://bsky.app/profile/someone.bsky.social/post/8m2tgw6cgi23i
"standard post text..."

Images created with the -b option can also be posted on Tumblr (bsky_post.py script not required).
Image file size upload limit for Bluesky is ~1MB.

)");
}
}  // namespace

namespace imgprmt {
std::optional<program_args> program_args::parse(int argc, char** argv) {
	auto arg = [&](int i) -> std::string_view {
		return (i >= 0 && i < argc) ? std::string_view(argv[i]) : std::string_view{};
	};

	const std::string prog = fs::path(argv[0]).filename().string();
	const std::string usage = std::format("Usage: {} [-b] <jpg_image>\n       {} --info", prog, prog);

	auto die = [&]() -> void { throw std::runtime_error(usage); };

	if (argc < 2 || argc > 3) {
		die();
	}
	if (argc == 2 && arg(1) == "--info") {
		display_info();
		return std::nullopt;
	}

	program_args out{};
	int i = 1;

	if (arg(i) == "-b") {
		out.option = Option::Bluesky;
		++i;
	}

	if (arg(i).empty()) {
		die();
	}
	out.image_file_path = fs::path(arg(i));
	if (i + 1 != argc) {
		die();
	}
	return out;
}
}  // namespace imgprmt
