
ofxFlowTools
============

openframeworks addon. Combines 2D fluid simulation, optical flow and more using GLSL shaders. Works best with live camera input.


INTRODUCTION

http://vimeo.com/92334462

For my interactive installations I was always attracted to fluid simulations. In the past I used the QuartzComposer fluid simulation, which I rewrote to openCL in Quartz Composer ( http://kineme.net/composition/MOostrik/AugmentedRealityinOpenCL ). Due to QC instability issues, especially with openCL (huge memory leaks) I decided to use my kernels in openframeworks using ofxMSAOpenCL. This worked quite nice (http://vimeo.com/46429663), but the process was quite difficult and debugging was a pain in the behind. When I was searching for alternatives, I started using ofxFX and ofxFluid, this convinced my to rewrite my openCL kernels to openGL shaders.

The addon is meant for developers who like to use fluid simulation in combination with a live camera input to create psychedelic live visuals. This is the reason i added optical flow and a simple mask, based on this flow. 
To understand the different stages of the fluid solver I wanted to be able to visualise all these stages. This meant i had to add a couple of extra draw buffers at the cost of a small performance hit. For optimization it will be more rewarding to work on the internal formats of the ofFbos. I was sad to discover there is only a limited amount of formats available. I ended up using GL_RGBA32F for most buffers where GL_RGBA16F would probably do as well. Even better would be to use GL_RG16F. Most  shaders I use are quite simple, so I expect the performance bottlenecks to be the fetching of the textures by the shaders.
I also wanted to have some simple draw functions for testing and completion. This turned out to be more time consuming than I expected, simple being the problem here. 
To complete the addon (what is a fluid sim without particles?) I added experimental GLSL particle engine.

Have fun!


DEPENDENCIES & SYSTEM SPECS

- The last version of the addon was created using OSX 10.10 and OF 0.8.4, using nVidia Geforce graphics cards.
- The addon should work on older versions of OSX, Windows and Linux, using nVidia Geforce, AMD Radeon cards.
- The addon might work on professional nVidia and AMD cards (it did work on the MacPro FirePro card, but not as fast as expected).
- The addon does NOT work on most integrated Intel graphics solutions, like the MacMini Iris graphics.



CREDITS AND ACKNOWLEDGEMENTS:

This addon started out as an expansion to ofxFluid by Patricio Gonzalez Vivo also using his ofxFX. While working on it it turned into a separate addon.

The Fluid shaders are adapted from various sources. Unfortunately I can't remember all, but the most important ones:
- Mark J Harris: Various online sources
- Patricio Gonzalez Vivo (http://www.patriciogonzalezvivo.com): ofxFluid

The Optical Flow Shader is based on a Quartz Composer patch, but was also inspired by ofxMIOFlowGLSL by Princemio

My interest in, and knowledge of, fluid simulation was hugely inspired by the work of Memo Akten. Thanx!


FUTURE ADDITIONS / TODO

first:
- Add help screen to the example
- Comment the code

second:
- Add Turing patterns: cake23.de/turing-fluid.html Beautiful!
- Enhance mask to only use colours from the movement (not the background)

last but not least:
- Create 3D fluid sim


(c) 2014 Matthias Oostrik
http://www.matthiasoostrik.com



