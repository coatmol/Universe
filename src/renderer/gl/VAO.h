#pragma once
#include <glad/glad.h>

#include "VBO.h"

class VAO
{
public:
	VAO();

	void LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset);
	void Bind();
	void Unbind();
	void Delete();

	GLuint ID;
private:
};