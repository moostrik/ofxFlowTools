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
out vec2 glFragColor;

uniform sampler2DRect tex_velocity;
uniform sampler2DRect tex_curl;

uniform float halfrdx;
uniform float timestep;
uniform float vorticity;

void main(){
	vec2 posn = texCoordVarying;
	
	// velocity
	vec2 vOld = texture(tex_velocity, posn).xy;
	
	// curl
	float cT = abs(textureOffset(tex_curl    , posn, + ivec2(0,1)).x);
	float cB = abs(textureOffset(tex_curl    , posn, - ivec2(0,1)).x);
	float cR = abs(textureOffset(tex_curl    , posn, + ivec2(1,0)).x);
	float cL = abs(textureOffset(tex_curl    , posn, - ivec2(1,0)).x);
	float cC =     texture      (tex_curl    , posn              ).x;
	
	// normalize
	vec2 dw = normalize(halfrdx * vec2(cT - cB, cR - cL) + 0.000001) * vec2(-1, 1);
	
	// vorticity confinement
	vec2 fvc = dw * cC *  timestep * vorticity;
	
	// add to velocity
	vec2 vNew = vOld + fvc;
	
	glFragColor = vNew;
}

