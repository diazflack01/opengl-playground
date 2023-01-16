#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main()
{
    // Default depth testing with texture
    FragColor = texture(texture0, TexCoord);

    // Non-linear depth testing
    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0);

    // Linear depth testing with near, far hard-coded for visualizing z-buffer
    // float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    // FragColor = vec4(vec3(depth), 1.0);
}
