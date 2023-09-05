# imgprmt

Command-line tool for embedding an image prompt / description within a JPG image file.  

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/purple.jpg)  

This creates a separate JPG + HTML polyglot image file.  
View the embedded prompt as a basic Web page just by renaming the *.jpg* file extension to *.htm*  

![Demo Image2](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/purple_html.png) 

The embedded image can be shared on a number of social media sites, such as Twitter, Mastodon,  
Flickr, Imgur & \*Reddit (\**Desktop only. Does not work with the Reddit mobile app*).  

These platforms will retain the embedded prompt, so it's available to anyone who downloads the image.  

In many cases, the embedded prompt is also preserved when someone edits the image.  

The embedded Web page with your image prompt is stored within the *ICC Profile* of the JPG image file.  

![Demo Image3](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/profile.png) 

*imgprmt* works on Linux and Windows.  

**AI images & prompts used as examples in this repo are the works of:**

*César Gámez [(@cesar20984)](https://twitter.com/cesar20984/status/1674961601101799430), Óscar Bartolomé [(@Artedeingenio)](https://twitter.com/Artedeingenio/status/1671079440107929602) & Orcton [(@OrctonAI)](https://twitter.com/OrctonAI)*

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

: https://twitter.com/cesar20984/status/1690816072306282497

Type or Paste in Your Image Prompt / Description

: A woman adorned with pink and purple botanical designs, resonating the style of
  rainforest-inspired, deep green and bright yellow, organically crafted body prints,
  impeccably detailed, utilizing flowers and vines, extremely meticulous,
  ultra-real patterns - ar 7:10

Created output file: "imgprmt_pic.jpg 189627 Bytes"

```

My other programs you may find useful:-  

* [pdvzip - PNG Data Vehicle (ZIP Edition) for Compatible Social Media & Image Hosting Sites.](https://github.com/CleasbyCode/pdvzip)  
* [jdvrif - JPG Data Vehicle for Reddit, Imgur, Flickr & Other Compatible Social Media / Image Hosting Sites.](https://github.com/CleasbyCode/jdvrif)
* [pdvrdt - PNG Data Vehicle for Reddit](https://github.com/CleasbyCode/pdvrdt)  
* [pdvps - PNG Data Vehicle for Twitter, PowerShell Edition](https://github.com/CleasbyCode/pdvps)   

##
