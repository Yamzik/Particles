#version 330 core
//in vec3 pos;
//in vec3 ourColor;
//in vec2 texCoord;
//
//uniform sampler2D texture0;
//uniform sampler2D texture1;
//uniform float mix_val;

out vec4 fc;

void main()
{
//	vec4 texvec0 = texture(texture0, texCoord);
//	vec4 texvec1 = texture(texture1, texCoord - 0.5);
//	fc = mix(texvec0, texvec1, mix_val);
	fc = vec4(ourColor, 1);
};