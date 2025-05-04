#pragma once
#include <glad/glad.h>

class EBO
{
public:
	EBO(GLuint* indices, GLsizeiptr size);
	
	void Bind();
	void Unbind();
	void Delete();

	GLuint ID;
private:
};