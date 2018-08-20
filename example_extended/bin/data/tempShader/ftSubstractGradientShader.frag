#version 150

uniform sampler2DRect Velocity;
uniform sampler2DRect Pressure;
uniform sampler2DRect Obstacle;
uniform float HalfInverseCellSize;

in vec2 texCoordVarying;
out vec4 fragColor;

void fTexNeighbors(sampler2DRect tex, vec2 st,
				   out float left, out float right, out float bottom, out float top) {
	left   = texture(tex, st - vec2(1, 0)).x;
	right  = texture(tex, st + vec2(1, 0)).x;
	bottom = texture(tex, st - vec2(0, 1)).x;
	top    = texture(tex, st + vec2(0, 1)).x;
}

void main(){
	
	vec2 st = texCoordVarying;
	
	float pL; float pR; float pB; float pT;
	fTexNeighbors (Pressure, st, pL, pR, pB, pT);
	float pC = texture(Pressure, st).x;
	
	float oL; float oR; float oB; float oT;
	fTexNeighbors (Obstacle, st, oL, oR, oB, oT);
	
	vec2 vMask = vec2(1.0,1.0);
	
	if (oL > 0.9) { pL = pC; vMask.x = 0.0; }
	if (oR > 0.9) { pR = pC; vMask.x = 0.0; }
	if (oB > 0.9) { pB = pC; vMask.y = 0.0; }
	if (oT > 0.9) { pT = pC; vMask.y = 0.0; }
	
	vec2 oldV = texture(Velocity, st).xy;
//	vec2 grad = vec2(pR - pL, pT - pB) * HalfInverseCellSize;
	vec2 grad = vec2(pR - pL, pT - pB) * 0.5;
	vec2 newV = oldV - grad;
	
	fragColor = vec4((vMask * newV), 0.0, 0.0);
}

