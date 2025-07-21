# imgprmt

***imgprmt*** is a command-line tool for storing and sharing your favorite AI image text prompts. The tool embeds your prompt into a ***JPG*** image and creates a unique ***JPG-HTML*** polyglot file, combining image, prompt text & html in a single output.  

Share the resulting image on compatible social media platforms like ***X-Twitter***, ***Tumblr***, ***Mastodon***, ***Flickr***, or ***Bluesky***, where the embedded prompt remains intact for anyone who downloads the image.  

To view the stored prompt locally, simply rename the image file extension to "*.htm*" and open it in a web browser to display it as a basic webpage.

*Try the imgprmt Web App [***here***](https://cleasbycode.co.uk/imgprmt/app/) for a convenient alternative to downloading and compiling the CLI source code.*

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/imgprmt_29443.jpg)  
*Image credit: [***@Dd41Giant***](https://x.com/Dd41Giant)*  

## Usage Example (Linux)

```console
user1@mx:~/Downloads/imgprmt-main/src$ sudo apt-get install libturbojpeg0-dev
user1@mx:~/Downloads/imgprmt-main/src$ chmod +x compile_imgprmt.sh
user1@mx:~/Downloads/imgprmt-main/src$ ./compile_imgprmt.sh
user1@mx:~/Downloads/imgprmt-main/src$ Compilation successful. Executable 'imgprmt' created.
user1@mx:~/Downloads/imgprmt-main/src$ sudo cp imgprmt /usr/bin

user1@mx:~/Desktop$ imgprmt 

Usage: imgprmt [-b|-x] <jpg_image> 
       imgprmt --info

user1@mx:~/Desktop$ imgprmt cosmic_garden.jpg

*** imgprmt v1.7 ***

Enter a Web link (Image source, Social media page, etc.)

Full URL Address: https://x.com/Dd41Giant/status/1945507214296273145

Type or paste in your prompt as one long sentence. Character limit: 4,096.

Avoid newline characters, instead add <br> tags for new lines, if required.

Image Description: Overhead Shot Extremely grainy photo taken with Lomo LC-A iso100 film...

Saved "prompt-embedded" JPG image: imgprmt_29443.jpg (170713 bytes).

```
Using the required, default command-line arguments without any options, the embedded image can only be posted on ***Tumblr***, ***Mastodon*** & ***Flickr***.  

The ***Linux*** command-line version of ***imgprmt*** is limited to *4096* characters for your prompt text.  The ***Windows*** command-line version and the ***imgprmt Web App*** has a larger prompt text limit of *59392* characters.

If you intend to share your *prompt-embedded* image on ***X-Twitter***, you ***must*** add the ***-x*** option to the command-line arguments:
```console
user1@mx:~/Desktop$ imgprmt -x my_image.jpg
```
The ***-x*** option reduces the prompt text limit to *6300* characters for the ***Windows*** command-line version and the ***imgprmt Web App*** (*4096 character limit for ***Linux****). This option also removes some ***JavaScript*** from the embedded webpage (*e.g. no clipboard copy button for the prompt*).  

These reductions allow the embedded image to be posted on ***X-Twitter***. You can also post these ***-x*** option images on ***Tumblr***, ***Mastodon*** & ***Flickr***.

If you intend to share your *prompt-embedded* image on ***Bluesky***, you ***must*** first add the ***-b*** option to the command-line arguments:  

```console
user1@mx:~/Desktop$ imgprmt -b my_image.jpg
```
Secondly, to post the image on ***Bluesky***, you will need to use the ***bsky_post.py*** script, *located in the repo ***src*** folder*. To run this script, you need ***Python3*** and the ***'requests'*** and ***'bs4' (BeautifulSoup) Python*** packages installed.  

An ***app-password*** is also required to use with the ***Python*** script, which you can create from your ***Bluesky*** account (*https://bsky.app/settings/app-passwords*).  

Below is a usage example for the ***bsky_post.py Python*** script:  

```console
$ python3 bsky_post.py --handle cleasbycode.bsky.social --password xxxx-xxxx-xxxx-xxxx --image imgprmt_21195.jpg --alt-text "Your_ALT-TEXT_here" "Your_standard_post_text_here"
```
Images created with the ***-b*** option can also be posted on ***Tumblr***.  Image file size limit for ***Bluesky*** is **1MB**.  

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

