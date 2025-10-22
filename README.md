# imgprmt

***imgprmt*** is a command-line tool for storing and sharing your favorite image text prompts/descriptions or even small articles. The tool embeds your prompt into a ***JPG*** image and creates a unique ***JPG-HTML*** polyglot file, combining image, prompt text & html in a single output.  

Any issues with configuring ***imgprmt***, then please just ask: [***@cleasbycode***](https://x.com/CleasbyCode).

Share the image on compatible social media platforms: ***X-Twitter***, ***Tumblr***, ***Mastodon***, ***Pixelfed***, ***Flickr*** & ****Bluesky***, where the embedded prompt remains intact for anyone who downloads the image.  

To view the stored prompt locally, simply rename the image file extension to "*.htm*" and open it in a web browser to display it as a basic webpage.

*Try the imgprmt Web App [***here***](https://cleasbycode.co.uk/imgprmt/app/) for a convenient alternative to downloading and compiling the CLI source code.*

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/imgprmt_55784.jpg)  
*Image credit: [***@_virtualvisions***](https://x.com/_virtualvisions/status/1907586383285301504)*  

## Usage Example (Linux)

```console
user1@mx:~/Downloads/imgprmt-main/src$ sudo apt-get install libturbojpeg0-dev
user1@mx:~/Downloads/imgprmt-main/src$ chmod +x compile_imgprmt.sh
user1@mx:~/Downloads/imgprmt-main/src$ ./compile_imgprmt.sh
user1@mx:~/Downloads/imgprmt-main/src$ Compilation successful. Executable 'imgprmt' created.
user1@mx:~/Downloads/imgprmt-main/src$ sudo cp imgprmt /usr/bin

user1@mx:~/Desktop$ imgprmt 

Usage: imgprmt [-b] <jpg_image> 
       imgprmt --info

user1@mx:~/Desktop$ imgprmt nurse.jpg

*** imgprmt v1.2 ***

Enter a web address (Your site, social media page, etc).
Full URL: https://x.com/_virtualvisions/status/1907586383285301504

Default byte limit: 57,084 | X-Twitter byte limit: 1,800.

Type or paste in your prompt as one long sentence.
If required, add <br> tags to your text for new lines.

Prompt: A woman dressed as a nurse with a syringe in her hand...

Saved "prompt-embedded" JPG image: imgprmt_32097.jpg (330511 bytes).

```
Using the default command-line arguments (no option), the embedded image can only be posted on ***Twitter***, ***Tumblr***, ***Mastodon*** & ***Flickr***.  

The default prompt limit is **57140** bytes (minus URL address length).
		
***X-Twitter***, for compatibility reasons, is limited to **1845** bytes (minus URL address length).  
Considerably smaller than the default size, but should be adequate for most prompts.

If you intend to share your *prompt-embedded* image on ***Bluesky***, you ***must*** first add the ***-b*** option to the command-line arguments:  

```console
user1@mx:~/Desktop$ imgprmt -b my_image.jpg
```
Secondly, to post the image on ***Bluesky***, you will need to use the ***bsky_post.py*** script, *located in the repo ***src*** folder*. To run this script, you need ***Python3*** and the ***'requests'*** and ***'bs4' (BeautifulSoup) Python*** packages installed.  

An ***app-password*** is also required, to use with the ***Python*** script, which you can create from your ***Bluesky*** account (*https://bsky.app/settings/app-passwords*).  

Here are some basic usage examples for the bsky_post.py Python script:  

Standard image post to your profile/account.

```console
$ python3 bsky_post.py --handle you.bsky.social --password xxxx-xxxx-xxxx-xxxx --image your_image.jpg --alt-text "alt-text here (optional)" "standard post text here (required)"
```
If you want to post multiple images (Max. 4).  

```console
$ python3 bsky_post.py --handle you.bsky.social --password xxxx-xxxx-xxxx-xxxx --image img1.jpg --image img2.jpg --alt-text "alt_here" "standard post text..."
```
If you want to post an image as a reply to another thread.  

```console
$ python3 bsky_post.py --handle you.bsky.social --password xxxx-xxxx-xxxx-xxxx --image your_image.jpg --alt-text "alt_here" --reply-to https://bsky.app/profile/someone.bsky.social/post/8m2tgw6cgi23i "standard post text..."
```

Images created with the ***-b*** option can also be posted on ***Tumblr***, (bsky_post script not required).
Image file size limit for ***Bluesky*** is **1MB**.  

https://github.com/user-attachments/assets/ecbae3b5-7252-4b2e-9249-64f334aad9ee

https://github.com/user-attachments/assets/805a0b8c-b397-4ba0-b6ca-d978a50e9bfb

https://github.com/user-attachments/assets/6942aa0b-4b4e-41ee-b58a-4318148b5952

## Third-Party Libraries

This project makes use of the following third-party libraries:

- libjpeg-turbo (see [***LICENSE***](https://github.com/libjpeg-turbo/libjpeg-turbo/blob/main/LICENSE.md) file)  
  - {This software is based in part on the work of the Independent JPEG Group.}
  - Copyright (C) 2009-2024 D. R. Commander. All Rights Reserved.
  - Copyright (C) 2015 Viktor Szathmáry. All Rights Reserved.
    
##
