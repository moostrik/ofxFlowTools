#version 150

uniform sampler2DRect Velocity;
uniform sampler2DRect Pressure;

in vec2 texCoordVarying;
out vec4 fragColor;

void main(){
	vec2 st = texCoordVarying;
	float pL = texture(Pressure, st - vec2(1, 0)).x;
	float pR = texture(Pressure, st + vec2(1, 0)).x;
	float pB = texture(Pressure, st - vec2(0, 1)).x;
	float pT = texture(Pressure, st + vec2(0, 1)).x;
	vec2 vel = texture(Velocity, st).xy;
	vec2 gradient = 0.5 * vec2(pR - pL, pT - pB);
	fragColor = vec4(vel - gradient, 0.0, 0.0);
}

