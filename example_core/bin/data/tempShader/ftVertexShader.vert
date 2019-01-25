#version 150
uniform mat4 modelViewProjectionMatrix;
uniform mat4 textureMatrix;

in vec4	position;
in vec2	texcoord;
in vec4	color;

out vec2 texCoordVarying;
out vec4 colorVarying;

void main()
{
	gl_Position = modelViewProjectionMatrix * position;
	texCoordVarying = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;
	colorVarying = color;
}
