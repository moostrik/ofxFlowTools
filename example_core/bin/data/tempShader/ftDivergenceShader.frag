/**
 *
 * PixelFlow | Copyright (C) 2016 Thomas Diewald - http://thomasdiewald.com
 *
 * A Processing/Java library for high performance GPU-Computing (GLSL).
 * MIT License: https://opensource.org/licenses/MIT
 *
 */


#version 410

precision mediump float;
precision mediump int;

in vec2 texCoordVarying;
out float glFragColor;

uniform sampler2DRect tex_velocity;
uniform sampler2DRect tex_obstacleC;
uniform sampler2DRect tex_obstacleN;

uniform float halfrdx;

void main(){
	
	vec2 posn = texCoordVarying;
	
	float oC = texture(tex_obstacleC, posn).x;
	if (oC == 1.0) {
		glFragColor = 0.0;
		return;
	}
	
	// velocity
	vec2 vT = textureOffset(tex_velocity, posn, + ivec2(0,1)).xy;
	vec2 vB = textureOffset(tex_velocity, posn, - ivec2(0,1)).xy;
	vec2 vR = textureOffset(tex_velocity, posn, + ivec2(1,0)).xy;
	vec2 vL = textureOffset(tex_velocity, posn, - ivec2(1,0)).xy;
	vec2 vC = texture      (tex_velocity, posn).xy;
	
	// no-slip (zero) velocity boundary conditions
	// use negative center velocity if neighbor is an obstacle
	vec4 oN = texture(tex_obstacleN, posn);
	vT = mix(vT, -vC, oN.x);  // if(oT > 0.0) vT = -vC;
	vB = mix(vB, -vC, oN.y);  // if(oB > 0.0) vB = -vC;
	vR = mix(vR, -vC, oN.z);  // if(oR > 0.0) vR = -vC;
	vL = mix(vL, -vC, oN.w);  // if(oL > 0.0) vL = -vC;
	
	glFragColor = halfrdx  * ((vR.x - vL.x) + (vT.y - vB.y));
}

