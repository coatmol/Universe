#pragma once

#include "gl/VAO.h"
#include "Shader.h"
#include "Camera.h"

class LineRenderer
{
public:
	LineRenderer(std::vector<GLfloat> verts);
	~LineRenderer();

	void Update(std::vector<GLfloat> verts);
	void Render(Shader& shader, Camera& camera);
private:
	VAO m_VAO;
	VBO* m_VBO;

	std::vector<GLfloat> m_Vertices;
};