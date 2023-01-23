#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

in VS_OUT {
    vec2 TexCoord;
} fs_in;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float screenWidth;
uniform float screenHeight;


void main()
{
    vec4 colorFrag = vec4(1.0, 1.0, 1.0, 1.0);

    if (gl_FrontFacing) {
        float scrHalfWidth = screenWidth / 2.0;
        float scrHalfHeight = screenHeight / 2.0;

        if (gl_FragCoord.x < scrHalfWidth) {
            if (gl_FragCoord.y < scrHalfHeight) {
                // bottom-left quadrant
                colorFrag = vec4(0.0, 0.0, 1.0, 1.0);
            } else {
                // top-left quadrant
                colorFrag = vec4(1.0, 0.0, 0.0, 1.0);
            }
        } else {
            if (gl_FragCoord.y < scrHalfHeight) {
                // bottom-right quadrant
                colorFrag = vec4(vec3(texture(texture0, fs_in.TexCoord)), 1.0);
            } else {
                // top-right quadrant
                colorFrag = vec4(0.0, 1.0, 0.0, 1.0);
            }
        }
    } else {
        // use different texture if backface
        colorFrag = vec4(vec3(texture(texture1, fs_in.TexCoord)), 1.0);
    }
    
    FragColor = colorFrag;
}
