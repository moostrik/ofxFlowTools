#version 150

uniform sampler2DRect Backbuffer;
uniform sampler2DRect Obstacle;
uniform sampler2DRect Velocity;

uniform float TimeStep;
uniform float Dissipation;
uniform float InverseCellSize;
uniform vec2	Scale;

in vec2 texCoordVarying;
out vec4 fragColor;

void main(){
	vec2 st = texCoordVarying;
	vec2 st2 = st * Scale;
	
	float inverseSolid = 1.0 - ceil(texture(Obstacle, st2).x - 0.5);
	
	vec2 u = texture(Velocity, st2).rg / Scale;
	vec2 coord =  st - TimeStep * InverseCellSize * u;
	
	fragColor = Dissipation * texture(Backbuffer, coord) * inverseSolid;
}

