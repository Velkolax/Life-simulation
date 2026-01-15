#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 HexColor;

uniform sampler2D image;

void main()
{
    vec4 texColor = texture(image, TexCoords);
    float gray = dot(texColor.rgb, vec3(0.2126, 0.7152, 0.0722)) * 1.2;
    vec3 coloredResult = gray * HexColor;
    FragColor = vec4(coloredResult, texColor.a);
}