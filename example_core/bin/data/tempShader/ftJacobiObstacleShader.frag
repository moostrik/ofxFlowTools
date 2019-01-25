#version 410
#define TINY 0.000001

uniform sampler2DRect Pressure;
uniform sampler2DRect Divergence;
uniform sampler2DRect Obstacle;

in vec2 texCoordVarying;
out vec4 fragColor;

void main() {
	vec2 st = texCoordVarying;
	
	vec2 offset = texture(Obstacle, st).xy;
	st+= offset;
	
	float D = texture(Divergence, st ).x;
	
	float pL = texture(Pressure, st - vec2(1, 0)).x;
	float pR = texture(Pressure, st + vec2(1, 0)).x;
	float pB = texture(Pressure, st - vec2(0, 1)).x;
	float pT = texture(Pressure, st + vec2(0, 1)).x;
	
	
	float alpha = -1;
	float beta = 0.25;
	float pres = (alpha * D + pL + pR + pB + pT) * beta;
	fragColor = vec4(pres, 0.0, 0.0, 0.0);
}
