#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
//uniform sampler2D texture1;
//uniform sampler2D texture2;

//uniform vec4 color;

float near = 0.1; 
    float far  = 100.0; 

    float LinearizeDepth(float depth) 
    {
        float z = depth * 2.0 - 1.0; // z powrotem do NDC
        return (2.0 * near * far) / (far + near - z * (far - near));	
    }

void main()
{
	//FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * color;
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // podziel przez far dla demonstracji
    FragColor = vec4(vec3(depth), 1.0);
}