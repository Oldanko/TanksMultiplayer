#version 430 core

in vec2 UV;

out vec4 outColor;

uniform sampler2D colormap;

void main()
{
	outColor = vec4(texture(colormap, UV).rgba);
}