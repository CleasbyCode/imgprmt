#include "information.h"

void displayInfo() {
	std::cout << R"(
Imgprmt v1.7 (CLI Edition). Created by Nicholas Cleasby (@CleasbyCode) 25/05/2023.

imgprmt is a command-line tool for Linux and Windows that embeds AI-generated image text prompts into JPG images. 
It creates a unique JPG-HTML polyglot file, combining image, prompt text & html in a single output.

Share the resulting image on compatible social media platforms like X-Twitter, Tumblr, Mastodon, Flickr, or Bluesky, 
where the embedded prompt remains intact for anyone who downloads the image.

To view the stored prompt locally, simply rename the image file extension to ".htm" and open
it in a web browser to display it as a basic webpage.

For a convenient alternative to downloading and compiling the CLI source code, try the imgprmt Web App:
https://cleasbycode.co.uk/imgprmt/app/

user1@mx:~/Desktop$ imgprmt 

Usage: imgprmt [-b|-x] <jpg_image> 
       imgprmt --info

With the default command-line arguments without any options, the embedded image can only be posted on Tumblr, Mastodon & Flickr.

The Linux command-line version of imgprmt is limited to 4096 characters for your prompt text.
The Windows command-line version and the imgprmt Web App has a larger prompt text limit of 59392 characters.

If you intend to share your prompt-embedded image on X-Twitter, you must add the -x option to the command-line arguments:
		
user1@mx:~/Desktop$ imgprmt -x my_image.jpg

The -x option reduces the prompt text limit to 6300 characters for the Windows CLI version and the imgprmt Web App (4096 character limit for Linux).
This option also removes some JavaScript from the embedded webpage (e.g. no clipboard copy button for the prompt).

These reductions allow the embedded image to be posted on X-Twitter. You can also post these -x option created images on Tumblr, Mastodon & Flickr.

If you intend to share your prompt-embedded image on Bluesky, you must first add the -b option to the command-line arguments:

user1@mx:~/Desktop$ imgprmt -b my_image.jpg

Secondly, to post the image on Bluesky, you will need to use the bsky_post.py script, located in the repo src folder.
To run this script, you need Python3 and the 'requests' and 'bs4' (BeautifulSoup) Python packages installed.

An app-password is also required to use with the Python script, which you can create from your Bluesky account: 
https://bsky.app/settings/app-passwords

Below is a usage example for the bsky_post.py Python script:

$ python3 bsky_post.py --handle cleasbycode.bsky.social --password xxxx-xxxx-xxxx-xxxx --image imgprmt_21195.jpg --alt-text "Your_ALT-TEXT_here" "Your_standard_post_text_here"

Images created with the -b option can also be posted on Tumblr. Image file size limit for Bluesky is 1MB.

)";
}
