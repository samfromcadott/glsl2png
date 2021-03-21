#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <lodepng.h>

#include <stdio.h>
#include <stdlib.h>

const char *vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"out vec2 pixCoord;\n"
	"void main() {\n"
	"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"   pixCoord = vec2(aPos.x+1.0, 1.0-aPos.y) / 2.0;\n"
	"}\0";

void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
	// Encode image
	unsigned error = lodepng_encode32_file(filename, image, width, height);

	// Display error message
	if(error) printf( "Error %u: %s\n", error, lodepng_error_text(error) );

}

int main(int argc, char const **argv) {
	printf("GLSL 2 PNG\n");
	int textureSize;

	if (argc != 4) {
		printf("Usage: glsl2png [GLSL source file] [PNG output file] [size]\n");
		return -1;

	}

	textureSize = atoi(argv[3]);

	// Open the GLSL source file
	char *fragSource = NULL;
	long length;
	FILE *f = fopen(argv[1], "r");

	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		fragSource = malloc(length);

		if (fragSource) {
			fread(fragSource, 1, length, f);

		}

		fclose (f);

	}

	if (!fragSource) {
		printf("Coundn't open file: %s\n", argv[1]);
		return -1;

	}

	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// Create Window
	GLFWwindow* window = glfwCreateWindow(1, 1, "GLSL 2 PNG", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;

	}

	glfwMakeContextCurrent(window);

	// GLAD
	if ( !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) ) {
		printf("Failed to initialize GLAD\n");
		return -1;

	}

	// Create the framebuffer and texture
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize, textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	// Compile Shaders

	// Vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		printf("Vertex shader compilation failed\n%s", infoLog);

	}

	// Fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		printf("Fragment shader compilation failed\n%s", infoLog);

	}

	free(fragSource);

	// Link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Shader linking failed failed\n%s", infoLog);

	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Create the square
	float vertices[] = {
		1.0f,  1.0f, 1.0f,  // top right
		1.0f, -1.0f, 1.0f,  // bottom right
		-1.0f, -1.0f, 1.0f,  // bottom left
		-1.0f,  1.0f, 1.0f   // top left
	};


	unsigned int indices[] = {
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Render
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
	glViewport(0, 0, textureSize, textureSize);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Save image
	GLubyte *image = malloc(textureSize * textureSize * 4);
	if (image == NULL) {
		printf("Couldn't create image.\n");
		return -1;

	}

	glReadPixels(0, 0, textureSize, textureSize, GL_RGBA, GL_UNSIGNED_BYTE, image);
	encodeOneStep(argv[2], image, textureSize, textureSize);
	free(image);

	glfwTerminate();
	return 0;

}
