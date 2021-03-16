#version 330 core
out vec4 FragColor;
in vec2 pixCoord;

void main() {
	FragColor = vec4(pixCoord, 0.0, 1.0f);

}
