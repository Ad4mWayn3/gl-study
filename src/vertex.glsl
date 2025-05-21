#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float redValue;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 vertColor;
out vec2 texCoord;

void main() {
	mat4 transform = projection * view * model;
//	transform = (transform * (1.0 - redValue)) + (mat4(1.) * redValue);

	gl_Position = transform * vec4(aPos, 1.0);
	texCoord = aTexCoord;
}
