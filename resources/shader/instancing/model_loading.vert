#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;

out vec3 Normal;
out vec3 FragPos;
out vec2 TextureCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
    Normal = aNormal;
    FragPos = vec3(instanceMatrix * vec4(aPos, 1.0));
    TextureCoords = aTexCoord;
}
