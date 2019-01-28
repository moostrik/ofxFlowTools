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
out vec2 glFragColor;

uniform sampler2DRect tex_velocity;
uniform sampler2DRect tex_temperature;
uniform sampler2DRect tex_density;

uniform float temperature_ambient;
uniform float timestep;
uniform float fluid_buoyancy;
uniform float fluid_weight;

void main(){
	vec2 posn = texCoordVarying;
	
	vec2  velocity    = texture(tex_velocity   , posn).xy;
	float temperature = texture(tex_temperature, posn).x;
	
	float dtemp = temperature - temperature_ambient;
	vec2 buoyancy = vec2(0.0);
	if (dtemp != 0.0) {
		float density = texture(tex_density, posn).a;
		float buoyancy_force = timestep * dtemp * fluid_buoyancy - density * fluid_weight;
		buoyancy = vec2(0, -1) * buoyancy_force;
	}
	
	glFragColor = buoyancy;
}

