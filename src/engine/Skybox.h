#pragma once
#include <string>

#include <glad/glad.h>
#include <stb_image.h>

#include "../renderer/Shader.h"
#include "../renderer/Camera.h"
#include "../renderer/gl/VAO.h"
#include "../renderer/gl/VBO.h"

class Skybox
{
public:
	Skybox(std::vector<std::string> filePath);

	void Render(Shader& shader, Camera& camera);
private:
	unsigned int m_TextureID;
	VAO m_VAO;
	VBO* m_VBO;
};