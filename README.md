# imgprmt

***imgprmt*** is a command-line tool for Linux & Windows, used to embed ***AI image text prompts*** within a ***JPG*** image.  The program outputs a separate ***JPG-HTML*** polyglot image file.  

You can share the image on a number of social media sites, such as ***X-Twitter***, ***Tumblr***, ***Mastodon***, ***Flickr*** & ****Bluesky***.  These platforms will retain the embedded prompt, so that it's available to anyone who downloads your image.  

The stored image prompt can be viewed locally as a basic web page. Simply rename the image file extension to *".htm"*, then open the file to view.

*You can try the [***imgprmt Web App, here,***](https://cleasbycode.co.uk/imgprmt/app/) if you don't want to download and compile the CLI source code.*  

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/imgprmt_48940.jpg)  
*Image credit: [***@daedalprincess***](https://x.com/daedalprincess)*  

## Usage Example (Linux)

```console
user1@mx:~/Downloads/imgprmt-main/src$ sudo apt-get install libturbojpeg-dev
user1@mx:~/Downloads/imgprmt-main/src$ chmod +x compile_imgprmt.sh
user1@mx:~/Downloads/imgprmt-main/src$ ./compile_imgprmt.sh
user1@mx:~/Downloads/imgprmt-main/src$ Compilation successful. Executable 'imgprmt' created.
user1@mx:~/Downloads/imgprmt-main/src$ sudo cp imgprmt /usr/bin

user1@mx:~/Desktop$ imgprmt 

Usage: imgprmt [-b] <jpg_image> 
       imgprmt --info

user1@mx:~/Desktop$ imgprmt black_gold.jpg

*** imgprmt v1.6 ***

Enter a Web link (Image source, Social media page, etc.)

Full URL Address: https://x.com/daedalprincess/status/1893078337817370801

Type or paste in your prompt as one long sentence.

Avoid newline characters, instead add <br> tags for new lines, if required.

Image Description: The image depicts a striking portrait of a woman with her face painted...
  
Created JPG-HTML polyglot image file: imgprmt_24799.jpg (95590 bytes).

```

If you intend to share your *prompt-embedded* image on ***Bluesky***, you ***must*** first add the ***-b*** option to the command-line arguments:
```console
user1@mx:~/Desktop$ imgprmt -b my_image.jpg
```
Secondly, to post the image on ***Bluesky***, you will need to use the ***bsky_post.py*** python script (*located in the repo src folder*). An App-Password is also required to use with the python script, which you can create from your Bluesky account (https://bsky.app/settings/app-passwords).  

Below is an usage example for the bsky_post.py python script:
```console
$python3 .\bsky_post.py --handle cleasbycode.bsky.social --password xxxx-xxxx-xxxx-xxxx --image imgprmt_21195.jpg --alt-text "Your_ALT-TEXT_here" "Your_standard_post_text_here"
```

*Prompt-embedded* images created with the ***-b*** option are only compatible for posting on the ***Bluesky*** platform. Image file size limit for ***Bluesky*** is **1MB**.

## Third-Party Libraries

This project makes use of the following third-party libraries:

- libjpeg-turbo (see [***LICENSE***](https://github.com/libjpeg-turbo/libjpeg-turbo/blob/main/LICENSE.md) file)  
  - {This software is based in part on the work of the Independent JPEG Group.}
  - Copyright (C) 2009-2024 D. R. Commander. All Rights Reserved.
  - Copyright (C) 2015 Viktor Szathmáry. All Rights Reserved.
    
##
