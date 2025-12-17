#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 HexColor;

uniform sampler2D image;

void main()
{
    vec4 texColor = texture(image, TexCoords);
    FragColor = texColor * vec4(HexColor, 1.0);
}