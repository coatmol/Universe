#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

out vec3 color;
out vec3 normal;
out vec3 fragPos;
out vec3 camPos;

uniform mat4 camMatrix;
uniform mat4 model;
uniform vec3 viewPos;

void main()
{
    gl_Position = camMatrix * model * vec4(aPos, 1);

    color = aColor;
    normal = mat3(transpose(inverse(model))) * aNormal;
    fragPos = vec3(model * vec4(aPos, 1));
    camPos = viewPos;
}