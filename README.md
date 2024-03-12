# imgprmt

Command-line tool for embedding an image prompt / description within a JPG image.  

This creates a separate JPG + HTML polyglot image file. 

Demo Videos: ***[Imgprmt Windows](https://youtu.be/sslNEuk3wz4) / [Imgprmt Web App](https://youtu.be/e4TdAxSCbCk)***  

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/dem_70440.jpg)  
*Image credit: Orcton [(@OrctonAI)](https://twitter.com/OrctonAI)*  

View the embedded prompt as a basic Web page by renaming the *.jpg* file extension to *.htm*  
![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/dem2_70440.jpg)  
*Image/prompt credit: Orcton [(@OrctonAI)](https://twitter.com/OrctonAI)*  

The embedded image can be shared on a number of social media sites, such as Twitter, Mastodon,  
Flickr, etc.  

These platforms retain the embedded prompt, so that it's available to anyone who downloads your image.  
In many cases, the embedded prompt is also preserved when someone edits the image.  

**Images & prompts used as examples in this repo are the works of:**
  
*(You can try **imgprmt** from this [**site**](https://cleasbycode.co.uk/imgprmt/index/) if you don't want to download & compile the source code.)*  

## Usage (Windows - Insert Prompt in JPG Image)

```console

PS G:\imgprmpt\demo> .\imgprmt.exe

Usage: imgprmt <jpg-image>  
       imgprmt --info

PS G:\imgprmpt\demo> .\imgprmt demo.jpg

Enter a Web link (Image source, Social media page, etc.)

URL: https://twitter.com/cesar20984/status/1690816072306282497

Type or paste in your prompt as one long sentence. Add <br> tags for new lines.

Image Decription: A woman adorned with pink and purple botanical designs, resonating the style of
  rainforest-inspired, deep green and bright yellow, organically crafted body prints,
  impeccably detailed, utilizing flowers and vines, extremely meticulous,
  ultra-real patterns - ar 7:10

Created output file: "imgprmt_27208.jpg 189627 Bytes"

```

My other programs you may find useful:-  

* [pdvzip: CLI tool to embed a ZIP file within a tweetable and "executable" PNG-ZIP polyglot image.](https://github.com/CleasbyCode/pdvzip)
* [jdvrif: CLI tool to encrypt & embed any file type within a JPG image.](https://github.com/CleasbyCode/jdvrif)
* [pdvrdt: CLI tool to encrypt, compress & embed any file type within a PNG image.](https://github.com/CleasbyCode/pdvrdt)
* [jzp: CLI tool to embed small files (e.g. "workflow.json") within a tweetable JPG-ZIP polyglot image.](https://github.com/CleasbyCode/jzp) 
* [pdvps: PowerShell / C++ CLI tool to encrypt & embed any file type within a tweetable and "executable" PNG image](https://github.com/CleasbyCode/pdvps)    

##
