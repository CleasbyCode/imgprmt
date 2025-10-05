# imgprmt

***imgprmt*** is a command-line tool for storing and sharing your favorite image text prompts. The tool embeds your prompt into a ***JPG*** image and creates a unique ***JPG-HTML*** polyglot file, combining image, prompt text & html in a single output.  

Share the image on compatible social media platforms like ***X-Twitter***, ***Tumblr***, ***Mastodon***, ***Flickr*** or ****Bluesky***, where the embedded prompt remains intact for anyone who downloads the image.  

To view the stored prompt locally, simply rename the image file extension to "*.htm*" and open it in a web browser to display it as a basic webpage.

*Try the imgprmt Web App [***here***](https://cleasbycode.co.uk/imgprmt/app/) for a convenient alternative to downloading and compiling the CLI source code.*

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/imgprmt_32097.jpg)  
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

*** imgprmt v2.1 ***

Enter a Web link (Image source, Social media page, etc.)

Full URL Address: https://x.com/_virtualvisions/status/1907586383285301504

Default character limit: 4,040 (X-Twitter character limit: 1,800).

Type or paste in your prompt as one long sentence.

Avoid newline characters, instead add <br> tags for new lines, if required.

Image Description: A woman dressed as a nurse with a syringe in her hand...

Saved "prompt-embedded" JPG image: imgprmt_32097.jpg (330511 bytes).

```
Using the default command-line arguments (no option), the embedded image can only be posted on ***Twitter***, ***Tumblr***, ***Mastodon*** & ***Flickr***.  

The ***Linux*** CLI version of ***imgprmt*** is limited to **4096** characters (minus ULR link) for your prompt text.  

The ***Windows*** CLI version and the ***imgprmt web app***, has a larger prompt text limit of **59392** characters (minus ULR link).

For compatibility reasons, ***X-Twitter*** has a reduced character limit of **1856** (minus URL link) for your prompt text.

If you intend to share your *prompt-embedded* image on ***Bluesky***, you ***must*** first add the ***-b*** option to the command-line arguments:  

```console
user1@mx:~/Desktop$ imgprmt -b my_image.jpg
```
Secondly, to post the image on ***Bluesky***, you will need to use the ***bsky_post.py*** script, *located in the repo ***src*** folder*. To run this script, you need ***Python3*** and the ***'requests'*** and ***'bs4' (BeautifulSoup) Python*** packages installed.  

An ***app-password*** is also required, to use with the ***Python*** script, which you can create from your ***Bluesky*** account (*https://bsky.app/settings/app-passwords*).  

Below is a usage example for the ***bsky_post.py Python*** script:  

```console
$ python3 bsky_post.py --handle cleasbycode.bsky.social --password xxxx-xxxx-xxxx-xxxx --image imgprmt_21195.jpg --alt-text "Your_ALT-TEXT_here" "Your_standard_post_text_here"
```
Images created with the ***-b*** option can also be posted on ***Tumblr***, (bsky_post script not required).
Image file size limit for ***Bluesky*** is **1MB**.  

https://github.com/user-attachments/assets/de24e5d6-39d5-4b09-a5f1-dbe5b2bbf8f5
  
https://github.com/user-attachments/assets/6f117d8b-815c-46ab-a89d-20022c8faa73

https://github.com/user-attachments/assets/2c810f39-cfcb-4357-a6b6-25a2dd5e6ea1

*Image credit: [***@Dd41Giant***](https://x.com/Dd41Giant/status/1940804031234167100)* 

## Third-Party Libraries

This project makes use of the following third-party libraries:

- libjpeg-turbo (see [***LICENSE***](https://github.com/libjpeg-turbo/libjpeg-turbo/blob/main/LICENSE.md) file)  
  - {This software is based in part on the work of the Independent JPEG Group.}
  - Copyright (C) 2009-2024 D. R. Commander. All Rights Reserved.
  - Copyright (C) 2015 Viktor Szathmáry. All Rights Reserved.
    
##
