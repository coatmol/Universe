#pragma once
#include <glad/glad.h>
#include <vector>

class VBO
{
public:
	VBO();
	VBO(GLfloat* vertices, GLsizeiptr size, GLenum type=GL_STATIC_DRAW);
	
	void Bind();
	void Update(GLfloat* vertices, GLsizeiptr size);
	void Unbind();
	void Delete();

	GLuint ID;
};