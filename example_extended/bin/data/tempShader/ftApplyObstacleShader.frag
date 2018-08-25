#version 150

uniform sampler2DRect	SrcTex;
uniform sampler2DRect	ObstacleTex;
uniform sampler2DRect	OffsetTex;

uniform float	Weight;
uniform vec2	Scale;

in vec2 texCoordVarying;
out vec4 fragColor;

void main()
{
	vec2 st = texCoordVarying;
	vec2 st2 = st * Scale;
	vec2 offset = texture(OffsetTex, st2).xy;
	float obstacle = texture(ObstacleTex, st2).x;
	vec4 src = texture(SrcTex, st + offset);
	if (length(offset) > 0) {
		fragColor = src * vec4((1.0 - obstacle) * Weight);
	}
	else {
		fragColor = src * vec4((1.0 - obstacle));
	}
}

