# imgprmt

***imgprmt*** is a command-line tool for Linux & Windows, used to embed ***AI image text prompts*** within a ***JPG*** image.  The program outputs a separate ***JPG-HTML*** polyglot image file.  

You can share the image on a number of social media sites, such as ***X-Twitter***, ***Tumblr***, ***Mastodon***, ***Flickr*** & ****Bluesky***.  These platforms will retain the embedded prompt, so that it's available to anyone who downloads your image.  

The stored image prompt can be viewed locally as a basic web page. Simply rename the image file extension to ".htm", then open the file to view.

*You can try the [***imgprmt Web App, here,***](https://cleasbycode.co.uk/imgprmt/app/) if you don't want to download and compile the CLI source code.*  

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/demo_image4.jpg) 

If you intend to share your *prompt-embedded* image on ***Bluesky***, you ***must*** first add the ***-b*** option to the CLI arguments:
```console
PS G:\demo> .\imgprmt -b my_image.jpg
```
Secondly, to post the image on ***Bluesky***, you will need to use the ***bsky_post.py*** python script (*located in the repo src folder*). An App-Password is also required to use with the python script, which you can create from your Bluesky account (https://bsky.app/settings/app-passwords).  

*Prompt-embedded* images created with the ***-b*** option are only compatible for posting on the ***Bluesky*** platform. Image file size limit for ***Bluesky*** is **1MB**.

![Demo Image](https://github.com/CleasbyCode/imgprmt/blob/main/demo_image/imgprmt_48940.jpg)  
*Image credit: [***@daedalprincess***](https://x.com/daedalprincess)* 

View the embedded prompt locally as a basic web page by renaming the ***.jpg*** file extension to ***.htm***  

## Usage (Windows CLI Default)

```console

PS G:\imgprmpt\demo> .\imgprmt.exe

Usage: imgprmt [-b] <jpg-image>  
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
## Usage (Windows CLI Bluesky Option and Upload Script)

```console

PS G:\imgprmpt\demo> .\imgprmt.exe

Usage: imgprmt [-b] <jpg-image>  
       imgprmt --info

PS G:\imgprmpt\demo> .\imgprmt -b Mansion_Monster.jpg

Enter a Web link (Image source, Social media page, etc.)

URL: https://x.com/CleasbyCode

Type or paste in your prompt as one long sentence. Add <br> tags for new lines, if required.

Image Description: In a grand yet dilapidated mansion...... 

Created output file: "imgprmt_21195.jpg 401529 Bytes"

PS G:\imgprmpt\demo> python3 .\bsky_post.py --handle cleasbycode.bsky.social --password xxxx-xxxx-xxxx-xxxx --image imgprmt_21195.jpg --alt-text "Testing prompt embedded image. To view the embedded prompt, save image, then change the .jpg file extension to .htm" Image_Test

```
https://github.com/user-attachments/assets/7fb4832c-f404-4e7a-8d2b-ea3ef1a4bf49  

https://github.com/user-attachments/assets/f31089c8-2ec3-47b5-91f9-9cb169fe2a49

## Third-Party Libraries

This project makes use of the following third-party libraries:

- libjpeg-turbo (see [***LICENSE***](https://github.com/libjpeg-turbo/libjpeg-turbo/blob/main/LICENSE.md) file)  
  - {This software is based in part on the work of the Independent JPEG Group.}
  - Copyright (C) 2009-2024 D. R. Commander. All Rights Reserved.
  - Copyright (C) 2015 Viktor Szathmáry. All Rights Reserved.
    
##

