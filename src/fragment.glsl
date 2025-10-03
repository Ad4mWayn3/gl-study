#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D tex2;
uniform float redValue;
uniform vec2 resolution;

void main() {
	vec4 texColor1 = texture(tex, texCoord);
	vec4 texColor2 = texture(tex2, texCoord);
	vec2 uv = gl_FragCoord.xy / resolution;

	FragColor = mix(texColor1, texColor2, uv.x);
}
