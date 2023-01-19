#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 viewPos;
uniform samplerCube skybox;
uniform sampler2D texture0;
uniform bool applyReflection = false;
uniform bool applyRefraction = false;

void main() {
    if (applyReflection) {
        vec3 viewToFragPosVec = normalize(FragPos - viewPos);
        vec3 viewToFragPosVecReflection = reflect(viewToFragPosVec, Normal);
        FragColor = vec4(vec3(texture(skybox, viewToFragPosVecReflection)), 1.0);
    } else if (applyRefraction) {
        vec3 viewToFragPosVec = normalize(FragPos - viewPos);
        const float ratio = 1.0 / 1.52; // object is in air being refracted to water environment
        vec3 viewToFragPosVecReflection = refract(viewToFragPosVec, Normal, ratio);
        FragColor = vec4(vec3(texture(skybox, viewToFragPosVecReflection)), 1.0);
    } else {
        FragColor = vec4(texture(texture0, TexCoord).xyz, 1.0);
    }
}
