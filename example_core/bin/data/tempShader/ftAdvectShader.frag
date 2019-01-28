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

uniform sampler2DRect tex_velocity;
uniform sampler2DRect tex_source;
uniform sampler2DRect tex_obstacleC;

uniform float timestep;
uniform float rdx;
uniform float dissipation;
uniform vec2  scale;

void main(){
	vec2 posn = texCoordVarying;
	
	float oC = texture(tex_obstacleC, posn).x;
	if (oC == 1.0) {
		glFragColor = vec4(0);
	} else {
		vec2 velocity = texture(tex_velocity, posn).xy;
		vec2 posn_back = posn - timestep * rdx * velocity;
		glFragColor = dissipation * texture(tex_source, posn_back);
	}
}

