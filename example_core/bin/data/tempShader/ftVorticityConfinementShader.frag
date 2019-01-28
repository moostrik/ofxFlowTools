#version 410
#define TINY 0.000001

uniform sampler2DRect Vorticity;

uniform float ConfinementScale;
uniform float Speed;

in vec2 texCoordVarying;
out vec4 fragColor;

void main(){
	vec2 st = texCoordVarying;
	float vorL = texture(Vorticity, st - vec2(1, 0)).x;
	float vorR = texture(Vorticity, st + vec2(1, 0)).x;
	float vorB = texture(Vorticity, st - vec2(0, 1)).x;
	float vorT = texture(Vorticity, st + vec2(0, 1)).x;
	float vorC = texture(Vorticity, st).x;
	
	vec2 dW = normalize((1.0 / Speed) * vec2(vorT - vorB, vorR - vorL) + TINY) * vec2(-1,1);
	vec2 force = dW * vorC * ConfinementScale * 2;
	
//	vec2 force = (1.0 / Speed) * vec2(abs(vorT) - abs(vorB), abs(vorR) - abs(vorL));
//	const float EPSILON = 2.4414e-4; // 2^-12
//	float magSqr = max(EPSILON, dot(force, force));
//	force *= inversesqrt(magSqr) * ConfinementScale * vorC * 2.0 * vec2(1., -1.);
	
	fragColor = vec4(force, 0.0, 0.0);
}
