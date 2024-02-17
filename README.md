# VOX
My first procedural world. Using OpenGL

## Building

```
$ git clone --recurse-submodules git@github.com:GitHuberlandYann/VOX.git
$ make setup
$ make
$ make log
```

## Features
### Infinite, procedurally generated world
### Saving and loading worlds
### Collision system
### Multithreading generation for smooth rendering
### Day/Night cycle
### Hunger system
### Different gamemodes
### Chat interface with own commands
### Transparent blocks
### 3D clouds
### Entities such as dropped items, flying arrows, falling blocks
### Particles system
### Animated fluids flow
### breaking animation
### Ambiant occlusion
### Distance fog
### Gravity-affected blocks (sand, gravel, lit tnts)
### UI with inventory
### Crafting system, storage system

## Libraries
* [GLFW](https://github.com/glfw/glfw.git) is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan application development. It provides a simple, platform-independent API for creating windows, contexts and surfaces, reading input, handling events, etc.
* [GLEW](https://github.com/nigels-com/glew.git) provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform.
* [GLM](https://github.com/g-truc/glm.git) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
* [SOIL](https://github.com/littlstar/soil.git) is a tiny C library used primarily for uploading textures into OpenGL.
