#version 150

uniform sampler2DRect Velocity;

in vec2 texCoordVarying;
out vec4 fragColor;

void main(){
	vec2 st = texCoordVarying;
	vec2 vL = texture(Velocity, st - vec2(1, 0)).xy;
	vec2 vR = texture(Velocity, st + vec2(1, 0)).xy;
	vec2 vB = texture(Velocity, st - vec2(0, 1)).xy;
	vec2 vT = texture(Velocity, st + vec2(0, 1)).xy;
	float vorticity = 0.5 * ((vR.y - vL.y) - (vT.x - vB.x));
	fragColor = vec4(vorticity, 0.0, 0.0, 0.0);
}
