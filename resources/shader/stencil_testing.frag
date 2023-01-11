#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform bool useUniformColor;
uniform vec3 color;
uniform sampler2D texture0;

void main()
{
    if (useUniformColor)
        FragColor = vec4(color, 1.0);
    else
        FragColor = texture(texture0, TexCoord);
}
