#version 330 core
in vec3 vertColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex;

void main() {
	vec4 texColor = texture(tex, texCoord);
	FragColor = mix(texColor, vec4(vertColor, 1.), 0.4);
}
