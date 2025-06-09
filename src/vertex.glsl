#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float redValue;
uniform vec3 camera;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 vertColor;
out vec2 texCoord;

void main() {
	mat4 transform = projection * view * model;

	gl_Position = transform * (vec4(aPos, 1.0) + vec4(-.3, -.3, 0., 0.));
	texCoord = aTexCoord;
}
