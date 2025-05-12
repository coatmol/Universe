#include "VBO.h"

VBO::VBO()
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

VBO::VBO(GLfloat* vertices, GLsizeiptr size, GLenum type)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, type);
}

void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Update(GLfloat* vertices, GLsizeiptr size)
{
	Bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
}

void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}
