#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

in VS_OUT {
    vec2 TexCoord;
} fs_in;

void main()
{
    if (gl_FrontFacing) {
        FragColor = vec4(vec3(texture(texture0, fs_in.TexCoord)), 1.0);
    } else {
        FragColor = vec4(vec3(texture(texture1, fs_in.TexCoord)), 1.0);
    }
}
