#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform bool useBlinn = false;
uniform bool useMaterialSpecular = false;

void main()
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoords))) * light.diffuse;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);

    float spec = 0.0;
    if(useBlinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, norm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    vec3 specular = spec * light.specular;
    if (useMaterialSpecular) {
        specular *= vec3(texture(material.specular, TexCoords));
    }

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}