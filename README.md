# imgprmt

Embed an AI image prompt within a JPG image file.  

This creates a separate JPG + HTML polyglot image file. 

Demo Videos: ***[Command Line](https://youtu.be/kzRwriXhPQE) / [Web](https://www.youtube.com/watch?v=vPGI48P9BtQ) / [X/Twitter](https://youtu.be/lJXOh-Zv9KQ)***  

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/demo_image4.jpg)  

View the embedded prompt as a basic web page by renaming the *.jpg* file extension to *.htm*  

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/demo_image6.png)   

The embedded image can be shared on a number of social media sites, such as X/Twitter, Tumblr, Mastodon,  
Flickr, etc.  

These platforms retain the embedded prompt, so that it's available to anyone who downloads your image.  
In many cases, the embedded prompt is also preserved when someone edits the image.  

*(You can try **imgprmt** from this [**site**](https://cleasbycode.co.uk/imgprmt/app/) if you don't want to download & compile the source code.)*  

## Usage (Windows Command Line - Insert Prompt in JPG Image)

```console

PS G:\imgprmpt\demo> .\imgprmt.exe

Usage: imgprmt <jpg-image>  
       imgprmt --info

PS G:\imgprmpt\demo> .\imgprmt Woman.jpg

Enter a Web link (Image source, Social media page, etc.)

URL: https://twitter.com/OrctonAI/status/1767377042029338722

Type or paste in your prompt as one long sentence. Add <br> tags for new lines, if required.

Image Description: A portrait of an enchanting young woman with long, wavy red hair standing in front of a field of colorful flowers at sunset.
 She has piercing blue eyes and is wearing a floral blouse. The sky above her features dark storm clouds, adding to the dramatic effect.
 Her expression is serene yet captivating as she gazes directly into the camera lens.
 The portrait is painted in the style of an artist known for dramatic lighting and expressive subjects. --ar 16:9 --stylize 125

Created output file: "imgprmt_5326.jpg 187573 Bytes"

```
## Usage (Web Tool)  
![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/imgprmt_screen.png) 

My other programs you may find useful:-  

* [pdvzip: CLI tool to embed a ZIP file within a tweetable and "executable" PNG-ZIP polyglot image.](https://github.com/CleasbyCode/pdvzip)
* [jdvrif: CLI tool to encrypt & embed any file type within a JPG image.](https://github.com/CleasbyCode/jdvrif)
* [pdvrdt: CLI tool to encrypt, compress & embed any file type within a PNG image.](https://github.com/CleasbyCode/pdvrdt)
* [jzp: CLI tool to embed small files (e.g. "workflow.json") within a tweetable JPG-ZIP polyglot image.](https://github.com/CleasbyCode/jzp) 
* [pdvps: PowerShell / C++ CLI tool to encrypt & embed any file type within a tweetable and "executable" PNG image](https://github.com/CleasbyCode/pdvps)    

##
