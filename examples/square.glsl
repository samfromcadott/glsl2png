#version 330 core
out vec4 FragColor;
in vec2 pixCoord;

void main() {
	vec3 color = vec3(0.0);

	vec2 bl = step(vec2(0.1), pixCoord);
	vec2 tr = step(vec2(0.1), 1.0-pixCoord);
	color = vec3(bl.x * bl.y * tr.x * tr.y);

	FragColor = vec4(color, 1.0f);

}
