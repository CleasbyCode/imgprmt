# imgprmt

This simple program embeds a prompt/description for your AI image within a JPG image file.

JPG / HTML Polyglot File.

The prompt text is saved within a basic HTML page inside the image file, which you can view  
just by renaming the .jpg file extension to .html

The image supports Twitter, Reddit & Imgur.  This means you can share your image on the these  
platforms and it will retain the embedded HTML image prompt.

imgprmt works on Linux and Windows.  

The embedded content is stored within the ICC Profile of the JPG image.

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/demo.jpg)  
{***Image credit: Orcton / @OrctonAI***}   

[**Video_Demo_1 - Insert AI Prompt as HTML Page Within JPG Image**](https://www.youtube.com/watch_popup?v=MiuSlScqcqc)  
[**Video_Demo_2 - Downloading from Twitter Image with Embedded Prompt**](https://www.youtube.com/watch_popup?v=OMHyhfDHoUQ)

Compile and run the program under Windows or **Linux**  

Image/Prompt credit:  
The AI image and prompt used in this repo demo is the work of [**César Gámez (@cesar20984)**](https://twitter.com/cesar20984/status/1662514596765216770)  

## Usage (Linux - Insert AI Prompt in JPG Image)

```c

$ g++ imgprmt.cpp -s -o imgprmt
$
$ ./imgprmt 

Usage:  imgprmt <jpg-image>  
        imgprmt --info

$ ./imgprmt  demo.jpg

Enter a URL (Image source, Social media page, etc.)

: https://twitter.com/OrctonAI/status/1664316306529697809

Type / Paste Your Image Prompt

: Create a powerful image from a cyberpunk film. A hacker, her face illuminated by the glow of multiple holographic screens, works in a high-tech lair. Her clothing, a blend of functional tech-wear and neon accents, stands out against the backdrop of the dark room. The room, filled with servers and tangled cables, hums with the sound of data being transferred at breakneck speed. The image captures the hacker's intense focus, her face conveying a myriad of emotions without uttering a single word. The scene is captured in intricate detail and sharp focus, highlighting the actor's expressive performance. 32k.  --ar 3:2 --v 5.1 --s 1000 --q 2

Created output file: "imgprmt_pic.jpg 198721 Bytes"

```

My other programs you may find useful:-  

* [pdvzip - PNG Data Vehicle for Twitter, ZIP Edition](https://github.com/CleasbyCode/pdvzip)  
* [jdvrdt - JPG Data Vehicle for Reddit](https://github.com/CleasbyCode/jdvrdt)
* [pdvrdt - PNG Data Vehicle for Reddit](https://github.com/CleasbyCode/pdvrdt)  
* [pdvps - PNG Data Vehicle for Twitter, PowerShell Edition](https://github.com/CleasbyCode/pdvps)   

##
