# imgprmt

*imgprmt* allows you to embed your AI image prompt within the JPG image file.

This creates a JPG / HTML polyglot file.

You can view the embedded prompt by just renaming the .jpg file extension to .html

The image is shareable via Twitter, Flickr, Reddit & Imgur.  These platforms will retain the embedded prompt.

*imgprmt* works on Linux and Windows.  

The embedded HTML page with the prompt is stored within the ICC Profile of the JPG image.

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/demo_.jpg)  
{***Image credit: Orcton / @OrctonAI***}  

![Html Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/html.jpg)  

**Video Demos**  

[**Embed Prompt in Image**](https://youtu.be/eHBGhOngMJo)  

[**Twitter Image with Embedded Prompt**](https://youtu.be/OMHyhfDHoUQ)        (Image/Prompt credit: César Gámez / [***@cesar20984***)](https://twitter.com/cesar20984/status/1662514596765216770)

[**Reddit Image with Embedded Prompt**](https://youtu.be/echWCq8PUxI)  

[**Imgur Image with Embedded Prompt**](https://youtu.be/yKaDx8tX1z4)  

Compile and run the program under Windows or **Linux**  

Image/Prompt credit:  
The AI image and prompt used in this repo demo is the work of [**Orcton (@OrctonAI)**](https://twitter.com/OrctonAI)  

## Usage (Linux - Insert AI Prompt in JPG Image)

```c

$ g++ imgprmt.cpp -s -o imgprmt
$
$ ./imgprmt 

Usage:  imgprmt <jpg-image>  
        imgprmt --info

$ ./imgprmt  demo.jpg

Enter a URL (Image source, Social media page, etc.)

: https://twitter.com/OrctonAI/status/1666119073875345409

Type / Paste Your Image Prompt

: A majestically cinematic close up professional photo quality shot of a masked villain at a creepy, 
abandoned carnival.  The villain's mask stands out against the faded colors of the dilapidated rides,
creating a disturbing image. The deserted carnival provides a haunting backdrop. This extremely detailed, 
magazine-quality photograph showcases expert precision, including intricate examples of undeniably amazing
image quality and detailed design. The scene is captured in stunning 8k sharp focus and 32k resolution.
--ar 3:2 --v 5.1 --s 1000 --q 2

Created output file: "imgprmt_pic.jpg 265122 Bytes"

```

My other programs you may find useful:-  

* [pdvzip - PNG Data Vehicle for Twitter, ZIP Edition](https://github.com/CleasbyCode/pdvzip)  
* [jdvrdt - JPG Data Vehicle for Reddit](https://github.com/CleasbyCode/jdvrdt)
* [pdvrdt - PNG Data Vehicle for Reddit](https://github.com/CleasbyCode/pdvrdt)  
* [pdvps - PNG Data Vehicle for Twitter, PowerShell Edition](https://github.com/CleasbyCode/pdvps)   

##
