#version 430 core
in vec2 particlePos;
in vec4 particleColor;

out vec4 fc;

uniform float diameter;
uniform float ratio;
uniform vec2 resolution;

void main()
{
	vec2 dot = gl_FragCoord.xy / resolution.xy - particlePos;

	if (pow(dot.x, 2) + pow(dot.y / ratio, 2) < pow(diameter / resolution.x / 2, 2))
		fc = vec4(particleColor);
	else discard;
};