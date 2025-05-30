#include "information.h"

void displayInfo() {
	std::cout << R"(
Imgprmt v1.6 (CLI Edition). Created by Nicholas Cleasby (@CleasbyCode) 25/05/2023.
		
This program enables you to embed an AI image text prompt within a JPG image file.
		
The program will output a JPG-HTML polyglot image file.
		
Your prompt text is saved within a basic HTML page embedded in the image file.
 
You can view the web page locally by simply renaming the ".jpg" file extension to ".htm".

The image can be shared on X-Twitter, Mastodon, Tumblr, Flickr & *Bluesky. 

*To create a compatible image for Bluesky, you first need to use the -b option with imgprmt.

Secondly, to post the image on Bluesky, you will need to use the bsky_post.py python script (located in the repo src folder). 

An App-Password is also required to use with the python script, which you can create from your Bluesky account (https://bsky.app/settings/app-passwords).

Below is an usage example for the bsky_post.py python script:

$ python3 bsky_post.py --handle cleasbycode.bsky.social --password xxxx-xxxx-xxxx-xxxx --image imgprmt_21195.jpg --alt-text "Your_ALT-TEXT_here" "Your_standard_post_text_here"

Image size limit for Bluesky is 1MB.

Note: Images created with the -b (Bluesky) option will only be compatible for that platform.

)";
}
