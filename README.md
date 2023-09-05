# imgprmt

Command-line tool for embedding an image prompt / description within a JPG image file.  

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/purple.jpg)  

This creates a separate JPG + HTML polyglot file.

View the embedded prompt as a basic Web page, by just renaming the *.jpg* file extension to *.htm*  

![Demo Image2](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/purple_html.png) 

The embedded image can be shared on a number of social media sites, such as Twitter, Mastodon,  
Flickr, Imgur & \*Reddit (\**Desktop only. Does not work with the Reddit mobile app*).  

These platforms will retain the embedded prompt, so it's available to anyone who downloads the image.  

Also, \*in many cases, the embedded prompt is also preserved when someone edits the image.  
*(\*not guaranteed / depends on image editor).*

The embedded Web page with the prompt / description is stored within the *iCC Profile* of the JPG image.

*imgprmt* works on Linux and Windows.  

**AI images & prompts used as examples in this repo are the works of:**

*César Gámez [(@cesar20984)](https://twitter.com/cesar20984/status/1674961601101799430), Óscar Bartolomé [(@Artedeingenio)](https://twitter.com/Artedeingenio/status/1671079440107929602) & Orcton [(@OrctonAI)](https://twitter.com/OrctonAI)*

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/screena.png)  

![Html Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/screenb.png)

***[Video Demo](https://youtu.be/sslNEuk3wz4)***
  
Compile and run the program under Windows or **Linux**  

## Usage (Linux - Insert AI Prompt in JPG Image)

```c

$ g++ imgprmt.cpp -s -o imgprmt
$
$ ./imgprmt 

Usage:  imgprmt <jpg-image>  
        imgprmt --info

$ ./imgprmt  demo.jpg

Enter a URL (Image source, Social media page, etc.)

: https://twitter.com/OrctonAI/status/1669135118806441987

Type / Paste Your Image Prompt

: A majestically cinematic professional photo quality shot of a beautiful space explorer on an alien
planet at high noon. Her semi-clothed form, accentuated by her tight-fitting space suit, adds to the
sense of adventure and exploration. The midday alien sun casts a dramatic light on her face,
highlighting her stunning eyes and the sense of wonder and discovery. The alien landscape,
the strange flora and fauna, and the distant mountains provide an epic backdrop to this scene.
This extremely detailed, magazine-quality photograph showcases expert precision, including intricate
examples of undeniably amazing image quality and detailed design. The scene is captured in stunning
8k sharp focus and 32k resolution. --ar 3:2 --v 5.1 --s 1000 --q 2

Created output file: "imgprmt_pic.jpg 189627 Bytes"

```

My other programs you may find useful:-  

* [pdvzip - PNG Data Vehicle (ZIP Edition) for Compatible Social Media & Image Hosting Sites.](https://github.com/CleasbyCode/pdvzip)  
* [jdvrif - JPG Data Vehicle for Reddit, Imgur, Flickr & Other Compatible Social Media / Image Hosting Sites.](https://github.com/CleasbyCode/jdvrif)
* [pdvrdt - PNG Data Vehicle for Reddit](https://github.com/CleasbyCode/pdvrdt)  
* [pdvps - PNG Data Vehicle for Twitter, PowerShell Edition](https://github.com/CleasbyCode/pdvps)   

##
