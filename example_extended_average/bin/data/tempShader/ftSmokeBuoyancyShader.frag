#version 150

uniform sampler2DRect Velocity;
uniform sampler2DRect Temperature;
uniform sampler2DRect Density;

uniform float AmbientTemperature;
uniform float TimeStep;
uniform float Sigma;
uniform float Kappa;

uniform vec2  Gravity;

in vec2 texCoordVarying;
out vec4 fragColor;

void main(){
	vec2 st = texCoordVarying;
	
	float T = texture(Temperature, st).r;
	vec2 vel = texture(Velocity, st).rg;
	float D = length(texture(Density, st).rgb);
	fragColor = vec4((TimeStep * (T - AmbientTemperature) * Sigma - D * Kappa ) * Gravity, 0.0, 0.0);
	}
