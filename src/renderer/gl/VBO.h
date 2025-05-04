#pragma once
#include <glad/glad.h>

class VBO
{
public:
	VBO(GLfloat* vertices, GLsizeiptr size);
	
	void Bind();
	void Unbind();
	void Delete();

	GLuint ID;
private:
};