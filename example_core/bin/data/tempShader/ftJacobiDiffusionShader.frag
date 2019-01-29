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
out vec4 glFragColor;

uniform sampler2DRect tex_source;
uniform sampler2DRect tex_obstacleC;
uniform sampler2DRect tex_obstacleN;

uniform float alpha;
uniform float beta;
uniform vec2  scale;

void main(){
	vec2 posn = texCoordVarying;
	vec2 posn2 = posn * scale;
	
	float oC = texture(tex_obstacleC, posn2).x;
	if (oC == 1.0) {
		glFragColor = vec4(0.0);
		return;
	}
	
	vec4 xT = textureOffset(tex_source, posn, + ivec2(0,1));
	vec4 xB = textureOffset(tex_source, posn, - ivec2(0,1));
	vec4 xR = textureOffset(tex_source, posn, + ivec2(1,0));
	vec4 xL = textureOffset(tex_source, posn, - ivec2(1,0));
	vec4 xC = texture      (tex_source, posn);
	
	vec4 oN = texture(tex_obstacleN, posn2);
	xT = mix(xT, xC, oN.x);
	xB = mix(xB, xC, oN.y);
	xR = mix(xR, xC, oN.z);
	xL = mix(xL, xC, oN.w);
	
	glFragColor = (xL + xR + xB + xT + alpha * xC) * beta;
}


