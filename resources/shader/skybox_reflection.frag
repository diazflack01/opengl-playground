#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main() {
    vec3 viewToFragPosVec = normalize(FragPos - viewPos);
    vec3 viewToFragPosVecReflection = reflect(viewToFragPosVec, Normal);
    FragColor = vec4(vec3(texture(skybox, viewToFragPosVecReflection)), 1.0);
}
