# simple_qt_renderrer

Is 3D renderer made from scratch on pure C++. Qt is using only for windowing and widgets (sliders, labels), it doesn't take part in rendering at all. This project was initially based on my previous work Simple_ASCII_Renderer.

## Motivation

This and previous CG projects were created to fulfill my interest in computer graphic programming. I was learning OpenGL at learnopengl.com and at some point i wasn't pretty sure about my chosen course because of lack of low-level knowledge. So I decided to make a renderer from scratch. While I was in midway, I had a computer graphics course at University, it made a influence on the project too.

## What's next

Now, this project is on hold for number of reasons:
* Renderer is not fast enough (my bad) and should be profiled deeper
* Qt is too big for this project, better choice SDL and Dear ImGui

But this project stays at GitHub to demonstrate my C++ and computer graphics knowledge.<br>
And now, as next stage, I want to deep dive into a Vulkan API.

## Features
* Shader-based forward renderer
* Programmable vertex & fragment shaders using C++ virtual functions
* Perspective correct vertex attributes interpolation
* Back-face culling
* Homogeneous near plane clipping
* Depth testing
* Blinn-Phong shading model
* Texture mapping 
* Sutherland-Hodgman mesh-mesh clipping
* Spatial bezier curve rendering
* Smooth bezier surface rendering

## Showcase

<img src="Examples\Barrel_Light_Flex.gif">
This image suffers a lot from GIF compression

## Dependencies
* [GLM](https://github.com/g-truc/glm) for math
* [stb_image](https://github.com/nothings/stb) for texture loading
* [assimp](https://github.com/assimp/assimp) for model loading
* [thread_pool](https://github.com/bshoshany/thread-pool) to parallelize rasterization

## References
* [scratchapixel](https://www.scratchapixel.com/)
* [learnopengl](https://learnopengl.com/)