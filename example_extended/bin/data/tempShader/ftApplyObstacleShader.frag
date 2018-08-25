#version 150

uniform sampler2DRect	SrcTex;
uniform sampler2DRect	ObstacleOffsetTex;

uniform float	Weight;
uniform vec2	Scale;

in vec2 texCoordVarying;
out vec4 fragColor;

void main()
{
	vec2 st = texCoordVarying;
	vec2 st2 = st * Scale;
	vec3 obs = texture(ObstacleOffsetTex, st2).xyz;
	vec2 offset = obs.xy;
	float obstacle = obs.z;
	vec4 src = texture(SrcTex, st + offset);
	if (length(offset) > 0) {
		fragColor = src * vec4(obstacle * Weight);
	}
	else {
		fragColor = src * vec4(obstacle);
	}
}

