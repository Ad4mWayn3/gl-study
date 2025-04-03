#version 330 core
in vec3 vertColor;
out vec4 FragColor;
uniform float redValue;

void main() {
	FragColor = vec4(vertColor, 1.0f);
}
