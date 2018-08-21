#version 150

uniform sampler2DRect Backbuffer;
uniform float Scale;

in vec2 texCoordVarying;
out vec4 fragColor;

void main(){
	vec2 st = texCoordVarying;
	fragColor = texture(Backbuffer, st) * vec4(Scale);
}

