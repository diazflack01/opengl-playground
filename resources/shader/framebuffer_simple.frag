#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture0;
uniform bool inversion = false;
uniform bool grayScaleAverage  = false;
uniform bool grayScaleWeighted  = false;
uniform bool kernelEffectSharpen  = false;
uniform bool kernelEffectBlurr  = false;
uniform float blurrStrength = 16.0;
uniform bool kernelEffectEdgeDetection = false;

vec3 calcKernelEffect(float kernel[9]);

void main()
{
    if (inversion) {
        FragColor = vec4(vec3(1 - texture(texture0, TexCoord)), 1.0);
    } else if (grayScaleAverage) {
        FragColor = texture(texture0, TexCoord);
        float average = (FragColor.r + FragColor.g + FragColor.b) / 3;
        FragColor = vec4(average, average, average, 1.0);
    } else if (grayScaleWeighted) {
        FragColor = texture(texture0, TexCoord);
        float weightedAverage = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
        FragColor = vec4(weightedAverage, weightedAverage, weightedAverage, 1.0);
    } else if (kernelEffectSharpen) {
        const float kernelSharpen[9] = float[](
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
        );
        FragColor = vec4(calcKernelEffect(kernelSharpen), 1.0);
    } else if (kernelEffectBlurr) {
        float kernelBlurr[9] = float[](
            1.0 / blurrStrength, 2.0 / blurrStrength, 1.0 / blurrStrength,
            2.0 / blurrStrength, 4.0 / blurrStrength, 2.0 / blurrStrength,
            1.0 / blurrStrength, 2.0 / blurrStrength, 1.0 / blurrStrength
        );
        FragColor = vec4(calcKernelEffect(kernelBlurr), 1.0);
    } else if (kernelEffectEdgeDetection) {
        const float kernelEdgeDetection[9] = float[](
            1,  1, 1,
            1, -8, 1,
            1,  1, 1
        );
        FragColor = vec4(calcKernelEffect(kernelEdgeDetection), 1.0);
    } else {
        FragColor = texture(texture0, TexCoord);
    }
}

vec3 calcKernelEffect(float kernel[9])
{
    const float offset = 1.0 / 300.0;

    const vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    vec3 sampledTexture[9];
    for (int i = 0; i < 9; i++) {
        sampledTexture[i] = vec3(texture(texture0, TexCoord.st + offsets[i]));
    }

    vec3 combined = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        combined += sampledTexture[i] * kernel[i];
    }

    return combined;
}
