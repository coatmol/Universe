#version 460 core  

out vec4 FragColor;  

in vec3 color;  
in vec3 normal;  
in vec3 fragPos;  
in vec3 camPos;  

void main()  
{  
   float ambientStrength = 0.0f;
   float specularStrength = 1.0f;

   vec3 lightPos = vec3(0.0f, 0.0f, 0.0f);
   vec3 lightColor = vec3(1.0f, 0.5f, 0.5f);

   vec3 ambient = ambientStrength * lightColor;

   vec3 norm = normalize(normal);
   vec3 lightDir = normalize(lightPos - fragPos);

   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;

   vec3 viewDir = normalize(camPos - fragPos);
   vec3 reflectDir = reflect(-lightDir, norm);

   float spec = 0.0;
   if (dot(norm, lightDir) > 0.0) {
       spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
   }
   vec3 specular = specularStrength * spec * lightColor;

   vec3 result = (ambient + diffuse + specular) * color;
   FragColor = vec4(result, 1.0f);
}