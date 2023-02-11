#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TextureCoords;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_diffuse4;
    sampler2D texture_diffuse5;
    sampler2D texture_diffuse6;
    sampler2D texture_diffuse7;
    sampler2D texture_diffuse8;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    sampler2D texture_specular4;
    sampler2D texture_specular5;
    sampler2D texture_specular6;
    sampler2D texture_specular7;
    sampler2D texture_specular8;
    float shininess;
    uint numDiffuse;
    uint numSpecular;
};

uniform Material material;

void main()
{
    FragColor = texture(material.texture_diffuse1, TextureCoords);
}
