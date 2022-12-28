#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TextureCoords;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight directionalLight;

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform SpotLight spotLight;

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = calcDirectionalLight(directionalLight, norm, viewDir);

    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir);

    result += calcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
    //FragColor = vec4(calcSpotLight(spotLight, norm, FragPos, viewDir), 1.0);
}

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TextureCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TextureCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TextureCoords));
    return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
  			     light.quadratic * (distance * distance));
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TextureCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TextureCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TextureCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TextureCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TextureCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TextureCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}