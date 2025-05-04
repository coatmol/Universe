#pragma once
#include <glad/glad.h>

#include "VBO.h"

class VAO
{
public:
	VAO();

	void LinkVBO(VBO& vbo, GLuint layout);
	void Bind();
	void Unbind();
	void Delete();
private:
	GLuint ID;
};