# imgprmt

Command-line tool for embedding an image prompt / description within a JPG image file.  

This creates a separate JPG + HTML polyglot image file. 

***[Imgprmt Windows CLI Demo](https://youtu.be/sslNEuk3wz4) / [Imgprmt Web Demo](https://youtu.be/FAKAwUJc-_Y)***  

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/purple.jpg)  
 
View the embedded prompt as a basic Web page by renaming the *.jpg* file extension to *.htm*  

![Demo Image2](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/purple_html.png) 

The embedded image can be shared on a number of social media sites, such as Twitter, Mastodon,  
Flickr, etc.  

These platforms retain the embedded prompt, so that it's available to anyone who downloads your image.  
In many cases, the embedded prompt is also preserved when someone edits the image.  

The embedded Web page with your image prompt is stored within the *ICC Profile* of the JPG image file.  

![Demo Image3](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/icc_img.png) 

**Images & prompts used as examples in this repo are the works of:**

*César Gámez [(@cesar20984)](https://twitter.com/cesar20984/status/1674961601101799430), Óscar Bartolomé [(@Artedeingenio)](https://twitter.com/Artedeingenio/status/1671079440107929602) & Orcton [(@OrctonAI)](https://twitter.com/OrctonAI)*

  
*(You can try **imgprmt** from this [**site**](https://cleasbycode.co.uk/imgprmt/index/) if you don't want to download & compile the source code.)*  

## Usage (Windows - Insert AI Prompt in JPG Image)

```console

PS G:\imgprmpt\demo> .\imgprmt.exe

Usage: imgprmt <jpg-image>  
       imgprmt --info

PS G:\imgprmpt\demo> .\imgprmt demo.jpg

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

* [pdvzip: CLI tool to embed a ZIP file within a tweetable and "executable" PNG-ZIP polyglot image.](https://github.com/CleasbyCode/pdvzip)
* [jdvrif: CLI tool to encrypt & embed any file type within a JPG image.](https://github.com/CleasbyCode/jdvrif)
* [pdvrdt: CLI tool to encrypt, compress & embed any file type within a PNG image.](https://github.com/CleasbyCode/pdvrdt)
* [jzp: CLI tool to embed small files (e.g. "workflow.json") within a tweetable JPG-ZIP polyglot image.](https://github.com/CleasbyCode/jzp) 
* [pdvps: PowerShell / C++ CLI tool to encrypt & embed any file type within a tweetable and "executable" PNG image](https://github.com/CleasbyCode/pdvps)    

##
