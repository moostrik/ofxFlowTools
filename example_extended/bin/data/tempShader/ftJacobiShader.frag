#version 150

uniform sampler2DRect Pressure;
uniform sampler2DRect Divergence;

in vec2 texCoordVarying;
out vec4 fragColor;

void main() {
	vec2 st = texCoordVarying;
	float pL = texture(Pressure, st - vec2(1, 0)).x;
	float pR = texture(Pressure, st + vec2(1, 0)).x;
	float pB = texture(Pressure, st - vec2(0, 1)).x;
	float pT = texture(Pressure, st + vec2(0, 1)).x;
	float div = texture(Divergence, st ).x;
	float alpha = -1;
	float beta = 0.25;
	float pres = (pL + pR + pB + pT + alpha * div) * beta;
	fragColor = vec4(pres, 0.0, 0.0, 0.0);
}

