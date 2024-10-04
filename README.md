# imgprmt

Use the command-line tool ***imgprmt*** to embed an AI image ***text prompt*** within a ***JPG*** image file.  

The program outputs a separate ***JPG+HTML*** polyglot image file. You can then share the image on a number of social media sites, such as ***X/Twitter***, ***Tumblr***, ***Mastodon***, ***Flickr***, etc.  

These platforms will retain the embedded prompt, so that it's available to anyone who downloads your image.   

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/demo_image4.jpg)  

To correctly download images from ***X/Twitter***, click the image in the post to ***fully expand*** it, before saving.  
View the embedded prompt as a basic web page by renaming the ***.jpg*** file extension to ***.htm***  

https://github.com/user-attachments/assets/7fb4832c-f404-4e7a-8d2b-ea3ef1a4bf49

## Usage (Windows Command Line - Insert Prompt in JPG Image)

```console

PS G:\imgprmpt\demo> .\imgprmt.exe

Usage: imgprmt <jpg-image>  
       imgprmt --info

PS G:\imgprmpt\demo> .\imgprmt Mansion_Monster.jpg

Enter a Web link (Image source, Social media page, etc.)

URL: https://x.com/CleasbyCode

Type or paste in your prompt as one long sentence. Add <br> tags for new lines, if required.

Image Description: In a grand yet dilapidated mansion interior with intricate wooden wall panelling
and a majestic staircase, an eerie, imposing figure standing on the stairs, lunges forward.<br><br>
This character is a blend of historical and fantastical elements, wearing a mask that evokes the textures
of burlap and bandages, accentuated with metal rivets and a sinister, grill-like mouthpiece.<br><br>
The eyes behind the mask are obscured by a cross-hatch pattern, enhancing its mysterious aura.
The attire is composed of ragged, earth-toned fabrics, suggesting a bygone era or a post-apocalyptic world.
<br><br>In one hand, it holds a blade, stained with blood, while the other hand reaches out toward the viewer
with a bloodied finger, threateningly pointed. The setting’s opulence clashes with the figure's menace, creating
a stark juxtaposition between the lavish environment and the character's foreboding presence.
<br><br>The lighting casts dramatic shadows, emphasizing the cinematic quality of the scene.
Cinematic and photographic styles. --ar 16:9

Created output file: "imgprmt_2547.jpg 401529 Bytes"

```
You can try **imgprmt** from this [***this site***](https://cleasbycode.co.uk/imgprmt/app/) if you don't want to download & compile the source code.

## Usage (Web Tool)  
![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/imgprmt_screen.png) 

My other programs you may find useful:-  

* [pdvzip: CLI tool to embed a ZIP file within a tweetable and "executable" PNG-ZIP polyglot image.](https://github.com/CleasbyCode/pdvzip)
* [jdvrif: CLI tool to encrypt & embed any file type within a JPG image.](https://github.com/CleasbyCode/jdvrif)
* [pdvrdt: CLI tool to encrypt, compress & embed any file type within a PNG image.](https://github.com/CleasbyCode/pdvrdt)
* [jzp: CLI tool to embed small files (e.g. "workflow.json") within a tweetable JPG-ZIP polyglot image.](https://github.com/CleasbyCode/jzp) 
* [pdvps: PowerShell / C++ CLI tool to encrypt & embed any file type within a tweetable and "executable" PNG image](https://github.com/CleasbyCode/pdvps)    

##
