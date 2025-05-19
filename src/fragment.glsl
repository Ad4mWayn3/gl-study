#version 330 core
in vec3 vertColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D tex2;
uniform float redValue;

void main() {
	vec4 texColor1 = texture(tex, texCoord);
	vec4 texColor2 = texture(tex2, texCoord);

	FragColor = mix(texColor1, texColor2, redValue);
}
