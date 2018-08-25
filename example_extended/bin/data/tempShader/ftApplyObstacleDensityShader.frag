#version 150

uniform sampler2DRect	SrcTex;
uniform sampler2DRect	ObstacleTex;

uniform vec2	Scale;

in vec2 texCoordVarying;
out vec4 fragColor;

void main()
{
	vec2 st = texCoordVarying;
	vec2 st2 = st * Scale;
	float obstacle = texture(ObstacleTex, st2).x;
	vec4 src = texture(SrcTex, st);
	fragColor = src * vec4((1.0 - obstacle));
}

