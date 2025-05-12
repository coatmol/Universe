#pragma once
#include <glad/glad.h>
#include <vector>

class EBO
{
public:
	EBO(GLuint* indices, GLsizeiptr size);
	
	void Bind();
	void Unbind();
	void Update(GLuint* vertices, GLsizeiptr size);
	void Delete();

	GLuint ID;
private:
};