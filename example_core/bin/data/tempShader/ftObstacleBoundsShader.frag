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

uniform sampler2DRect tex_obstacleC;

void main(){
	vec2 posn = texCoordVarying;
	glFragColor = vec4(0);
	glFragColor.x = textureOffset(tex_obstacleC, posn, + ivec2(0,1)).x;
	glFragColor.y = textureOffset(tex_obstacleC, posn, - ivec2(0,1)).x;
	glFragColor.z = textureOffset(tex_obstacleC, posn, + ivec2(1,0)).x;
	glFragColor.w = textureOffset(tex_obstacleC, posn, - ivec2(1,0)).x;
}

