#version 430 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aVelocity;
layout(location = 2) in vec2 aForce;
layout(location = 3) in vec2 aColorRG;
layout(location = 4) in vec2 aColorBA;
layout(location = 5) in float z;

out vec2 particlePos;
out vec4 particleColor;

uniform float diameter;

void main()
{
	vec2 actualPos = (aPos + 1.0) / 2.0;
	gl_PointSize = diameter;
	gl_Position = vec4(aPos, z, 1.0);
	particlePos = actualPos;
	particleColor = vec4(aColorRG, aColorBA);
}