#version 460 core

out vec4 FragColor;

in vec3 color;

void main()
{
    float ambientStrength = 0.1f;
    vec3 lightColor = vec3(1.0f, 0.0f, 0.0f);
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * color;
    FragColor = vec4(result, 1.0f);
} 