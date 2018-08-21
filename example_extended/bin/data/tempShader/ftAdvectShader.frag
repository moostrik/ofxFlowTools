#version 150

uniform sampler2DRect Backbuffer;
uniform sampler2DRect Velocity;

uniform float	TimeStep;
uniform float	Dissipation;
uniform vec2	Scale;

in vec2 texCoordVarying;
out vec4 fragColor;

void main(){
	vec2 st = texCoordVarying;
	vec2 st2 = st * Scale;
	
	vec2 u = texture(Velocity, st2).rg / Scale;
	vec2 coord =  st - TimeStep * u;
	
	fragColor = Dissipation * texture(Backbuffer, coord);
}

