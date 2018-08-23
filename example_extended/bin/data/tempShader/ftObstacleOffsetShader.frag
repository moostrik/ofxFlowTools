#version 150

uniform sampler2DRect	tex0;

in vec2 texCoordVarying;
out vec4 fragColor;

void main()
{
	vec2 st = texCoordVarying;
	float off = 1;
	vec2 off_x = vec2(off, 0.0);
	vec2 off_y = vec2(0.0, off);
	
	//calculate the gradient
	float gradx; float grady; float gradmag;
	gradx = texture(tex0, st + off_x).x - texture(tex0, st - off_x).x;
	grady = texture(tex0, st + off_y).x - texture(tex0, st - off_y).x;
	gradmag = sqrt((gradx*gradx) + (grady*grady) + 0.0001);
	
	vec2 offset;
	float scr = texture(tex0, st).x;
	offset.x = scr*(gradx/gradmag);
	offset.y = scr*(grady/gradmag);
//	offset.x = round(gradx/gradmag);
//	offset.y = round(grady/gradmag);
//
	fragColor = vec4(offset, 0.0, 1.0);
}

