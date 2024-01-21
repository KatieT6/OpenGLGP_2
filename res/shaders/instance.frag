#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec4 dynamicColor;
uniform vec3 viewPos;

#define NO_POINT_LIGHTS 1
#define NO_SPOT_LIGHTS 2


struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool enabled;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool enabled;

};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool enabled;
};

uniform DirLight dirLight;
uniform SpotLight spotLights[NO_SPOT_LIGHTS];
uniform PointLight pointLights[NO_POINT_LIGHTS];


void main()s
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 specular = vec3(0.0);
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    // kierunkowe
    if (dirLight.enabled)  // Check if the spot light is enabled
        {
        vec3 lightDir = normalize(-dirLight.direction);
        float diff = max(dot(norm, lightDir), 0.0); //lambert
        ambient = dirLight.ambient * texture(texture_diffuse1, TexCoords).rgb;
        diffuse = dirLight.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), 16.0);//phong
        specular = dirLight.specular * spec;
    }
    // reflektorowe
    vec3 spotEffect = vec3(0.0);
    for (int i = 0; i < 2; i++)
    {
        if (spotLights[i].enabled)  // Check if the spot light is enabled
        {
            vec3 spotDir = normalize(spotLights[i].position - FragPos);
            float theta = dot(spotDir, normalize(-spotLights[i].direction));
            if (theta > spotLights[i].outerCutOff)
            {
                float intensity = clamp((theta - spotLights[i].outerCutOff) / (spotLights[i].cutOff - spotLights[i].outerCutOff), 0.0, 1.0);
                float spotDiff = max(dot(norm, spotDir), 0.0);
                vec3 spotAmbient = spotLights[i].ambient * texture(texture_diffuse1, TexCoords).rgb;
                vec3 spotDiffuse = spotLights[i].diffuse * spotDiff * texture(texture_diffuse1, TexCoords).rgb;
                vec3 spotHalfwayDir = normalize(spotDir + viewDir);
                float spotSpec = pow(max(dot(norm, spotHalfwayDir), 0.0), 16.0);
                vec3 spotSpecular = spotLights[i].specular * spotSpec;
                spotEffect += (spotAmbient + spotDiffuse + spotSpecular) * intensity;
            }
        }
    }

    // punktowe
    vec3 pointEffect = vec3(0.0);
    if (pointLights[0].enabled)  // Check if the point light is enabled
    {
        vec3 pointDir = normalize(pointLights[0].position - FragPos);
        float distance = length(pointLights[0].position - FragPos);
        float attenuation = 1.0 / (pointLights[0].constant + pointLights[0].linear * distance + pointLights[0].quadratic * (distance * distance));
        float pointDiff = max(dot(norm, pointDir), 0.0);
        vec3 pointAmbient = pointLights[0].ambient * texture(texture_diffuse1, TexCoords).rgb;
        vec3 pointDiffuse = pointLights[0].diffuse * pointDiff * texture(texture_diffuse1, TexCoords).rgb;
        vec3 pointHalfwayDir = normalize(pointDir + viewDir);
        float pointSpec = pow(max(dot(norm, pointHalfwayDir), 0.0), 16.0);
        vec3 pointSpecular = pointLights[0].specular * pointSpec;
        pointEffect = (pointAmbient + pointDiffuse + pointSpecular) * attenuation;
    }

    // razem
    vec3 result = ambient + diffuse + specular + spotEffect + pointEffect;
    FragColor = vec4(result, 1.0);
}