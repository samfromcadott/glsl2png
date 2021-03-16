GLSL 2 PNG
==========

A command line program that renders the output of a GLSL fragment shader as a PNG image. This is a proof of concept for making a program that generates PBR textures using GLSL.

Usage
----------

	glsl2png [GLSL source file] [PNG output file]

Requirements
----------

* lodepng
* glad
* GLFW
* KHR

Building
----------

1. Place the required libraries in `include` and `lib` and put `glad.c` and `lodepng.c` in `src`
2. Run `make`
