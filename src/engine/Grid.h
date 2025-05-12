#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include <glad/glad.h>

#include "../renderer/gl/VAO.h"
#include "../renderer/Shader.h"
#include "../renderer/Camera.h"
#include "Body.h"

class Grid
{
public:
	Grid(float size, int divisions);

	void Update(const std::vector<Body*> bodies);
	void Render(Shader& shader, Camera& camera);

private:
	VAO m_VAO;
	VBO* m_VBO;

	std::vector<GLfloat> m_Vertices;
	std::vector<GLfloat> m_OgVerts;
};