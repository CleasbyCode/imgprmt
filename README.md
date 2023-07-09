# imgprmt

*imgprmt* allows you to embed your AI image prompt within the JPG image file.

This creates a JPG / HTML polyglot file.

You can view the embedded prompt by just renaming the .jpg file extension to .html

The image is shareable via Twitter, Mastodon, Flickr, Reddit & Imgur.  These platforms will retain the embedded prompt.

*imgprmt* works on Linux and Windows.  

The embedded HTML page with the prompt is stored within the ICC Profile of the JPG image.

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/screen1.jpg)  
{***Image credit: Orcton / @OrctonAI***}  

![Html Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/screen.jpg)

**Video Demos**  

* [***Insert Prompt Inside JPG Image Using Imprmt***](https://youtu.be/KKpEt6F0r6I)  

* [***Twitter Image with Embedded Prompt***](https://youtu.be/TS3mlyBQey4)  

* [***Mastodon Image with Embedded Prompt***](https://youtu.be/0LcQD-axERg)

* [***Reddit Image with Embedded Prompt***](https://youtu.be/aM5sFTyG2no)  

Compile and run the program under Windows or **Linux**  

Image/Prompt credit:  
The AI image and prompt used in this repo demo is the work of [**@OrctonAI**](https://twitter.com/OrctonAI)  

## Usage (Linux - Insert AI Prompt in JPG Image)

```c

$ g++ imgprmt.cpp -s -o imgprmt
$
$ ./imgprmt 

Usage:  imgprmt <jpg-image>  
        imgprmt --info

$ ./imgprmt  demo.jpg

Enter a URL (Image source, Social media page, etc.)

: https://twitter.com/OrctonAI/status/1672705138132230144 

Type / Paste Your Image Prompt

: majestically cinematic close up professional photo quality shot of a futuristic intense scene
from a sci-fi film featuring a lone astronaut stranded on an alien planet. very futuristic with
LED’s and advance tech. The astronaut stumbles upon a peculiar yet amazing alien life form, first
contact with an intelligent alien species. The foreign landscape around them is bathed in the
eerie glow of the planet's twin moons. extremely detailed magazine quality photograph, cinematic
lighting, 8k sharp focus. expert precision including intricate examples of undeniably amazing
image quality and detailed design, stunning unique with intricate details, amazing
background detail. Sharp focus, intricate detail, 32k --weird 250 --ar 16:9 --s 250

Created output file: "imgprmt_pic.jpg 265122 Bytes"

```

My other programs you may find useful:-  

* [pdvzip - PNG Data Vehicle (ZIP Edition) for Compatible Social Media & Image Hosting Sites.](https://github.com/CleasbyCode/pdvzip)  
* [jdvrif - JPG Data Vehicle for Reddit, Imgur, Flickr & Other Compatible Social Media / Image Hosting Sites.](https://github.com/CleasbyCode/jdvrif)
* [pdvrdt - PNG Data Vehicle for Reddit](https://github.com/CleasbyCode/pdvrdt)  
* [pdvps - PNG Data Vehicle for Twitter, PowerShell Edition](https://github.com/CleasbyCode/pdvps)   

##
