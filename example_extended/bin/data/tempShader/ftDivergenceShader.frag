#version 150

uniform sampler2DRect Velocity;
uniform float HalfInverseCellSize;

in vec2 texCoordVarying;
out vec4 fragColor;

void main(){
	vec2 st = texCoordVarying;
	vec2 vL = texture(Velocity, st - vec2(1, 0)).xy;
	vec2 vR = texture(Velocity, st + vec2(1, 0)).xy;
	vec2 vB = texture(Velocity, st - vec2(0, 1)).xy;
	vec2 vT = texture(Velocity, st + vec2(0, 1)).xy;
	float divergence = 0.5 * (vR.x - vL.x + vT.y - vB.y);
	fragColor = vec4(divergence, 0, 0, 0);
}

