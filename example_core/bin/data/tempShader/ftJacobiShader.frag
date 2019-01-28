/**
 *
 * PixelFlow | Copyright (C) 2016 Thomas Diewald - http://thomasdiewald.com
 *
 * A Processing/Java library for high performance GPU-Computing (GLSL).
 * MIT License: https://opensource.org/licenses/MIT
 *
 */


#version 150

precision mediump float;
precision mediump int;

in vec2 texCoordVarying;
out vec4 glFragColor;

uniform sampler2DRect tex_x;
uniform sampler2DRect tex_b;
uniform sampler2DRect tex_obstacleC;
uniform sampler2DRect tex_obstacleN;

uniform float alpha;
uniform float rBeta;

void main(){
	
	vec2 posn = texCoordVarying;
	
	float oC = texture(tex_obstacleC, posn).x;
	if (oC == 1.0) {
		glFragColor = vec4(0);
		return;
	}
	
	// tex b
	vec4 bC = texture(tex_b, posn);
	
	// tex x
	vec4 xT = textureOffset(tex_x, posn, + ivec2(0,1));
	vec4 xB = textureOffset(tex_x, posn, - ivec2(0,1));
	vec4 xR = textureOffset(tex_x, posn, + ivec2(1,0));
	vec4 xL = textureOffset(tex_x, posn, - ivec2(1,0));
	vec4 xC = texture      (tex_x, posn);
	
	// pure Neumann pressure boundary
	// use center x (pressure) if neighbor is an obstacle
	vec4 oN = texture(tex_obstacleN, posn);
	xT = mix(xT, xC, oN.x);  // if (oT > 0.0) xT = xC;
	xB = mix(xB, xC, oN.y);  // if (oB > 0.0) xB = xC;
	xR = mix(xR, xC, oN.z);  // if (oR > 0.0) xR = xC;
	xL = mix(xL, xC, oN.w);  // if (oL > 0.0) xL = xC;
	
	glFragColor = (xL + xR + xB + xT + alpha * bC) * rBeta;
}



