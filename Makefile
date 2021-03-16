BINARY = glsl2png
BUILDDIR = ./bin

CC = gcc
CFLAGS = -Wall -Wextra -O2
CINCLUDES = -I./include
CLIB = -L./lib

$(BINARY):
	$(CC) $(CFLAGS) $(CINCLUDES) $(CLIB) src/glad.c src/main.c src/lodepng.c -o $(BUILDDIR)/$@ -lglfw3 -lgdi32 -lopengl32 -lglu32
