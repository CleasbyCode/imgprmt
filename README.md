# imgprmt

This simple program enables you to embed a prompt/description for your AI image within a JPG image file.

JPG / HTML Polyglot File.

The prompt is saved within a basic HTML page inside the image file, which you can view 
just by renaming the .jpg file extension to .html

The image supports Twitter, Reddit & Imgur.  
This means you can share your image on the above platforms and it will retain the embedded HTML image prompt.

imgprmt works on Linux and Windows.  
The embedded content is stored within the ICC Profile of the JPG image.

![Demo Image](https://github.com/CleasbyCode/jdvrdt/blob/main/demo_image/Demo.jpg)  
{***Image credit: César Gámez (@cesar20984)***}   

[**Video_Demo_1 (YouTube) - Insert AI Prompt as HTML Page Within JPG Image**](https://www.youtube.com/watch_popup?v=MiuSlScqcqc)  
[**Video_Demo_2 (YouTube) - Downloading from Twitter Image with Embedded Prompt**](https://www.youtube.com/watch_popup?v=OMHyhfDHoUQ)

Compile and run the program under Windows or **Linux**.

Image/Prompt credit: The AI image and prompt used in this repo demo is the work of [**César Gámez (@cesar20984)**](https://twitter.com/cesar20984/status/1662514596765216770)

## Usage (Linux - Insert AI Prompt in JPG Image)

```c

$ g++ imgprmt.cpp -s -o imgprmt
$
$ ./imgprmt 

Usage:  imgprmt <jpg-image>  
        imgprmt --info

$ ./imgprmt  demo.jpg

Type / Paste Your Image Description

: A mysterious traveler draped in a hooded cloak, holding an ancient staff adorned with  
mystical  symbols, stands at the edge of a vibrant emerald cliff overlooking a sprawling  
cityscape, where futuristic skyscrapers blend seamlessly with nature as rivers flow around  
and between the structures. The clouds above shift with vibrant shades of pink and purple,  
mirroring the hues of the setting sun in the distance. The traveler gazes upon the city,  
sensing that their long quest will come to a conclusion within its walls, in the style  
of CyberFantasyCore, 8k resolution --ar 16:9

Enter a Link (Image Source, Social Media Page, etc.)

: https://twitter.com/cesar20984/status/1662514596765216770  
  
Created output file: "imgprmt_pic.jpg 425108 Bytes"

```

My other programs you may find useful:-  

* [pdvzip - PNG Data Vehicle for Twitter, ZIP Edition](https://github.com/CleasbyCode/pdvzip)  
* [jdvrdt - JPG Data Vehicle for Reddit](https://github.com/CleasbyCode/jdvrdt)
* [pdvrdt - PNG Data Vehicle for Reddit](https://github.com/CleasbyCode/pdvrdt)  
* [pdvps - PNG Data Vehicle for Twitter, PowerShell Edition](https://github.com/CleasbyCode/pdvps)   

##
