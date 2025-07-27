#include "LineRenderer.h"

LineRenderer::LineRenderer(std::vector<GLfloat> verts)
	: m_Vertices(verts)
{
	m_VAO = VAO();
	m_VAO.Bind();

	m_VBO = new VBO(m_Vertices.data(), GLsizeiptr(m_Vertices.size() * sizeof(GLfloat)), GL_DYNAMIC_DRAW);

	m_VAO.LinkAttrib(*m_VBO, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	m_VAO.LinkAttrib(*m_VBO, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	m_VAO.Unbind();
	m_VBO->Unbind();
}

LineRenderer::~LineRenderer()
{
	m_VAO.Delete();
	m_VBO->Delete();
}

void LineRenderer::Update(std::vector<GLfloat> verts)
{
	m_Vertices = verts;
	m_VBO->Bind();
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(GLfloat), m_Vertices.data(), GL_DYNAMIC_DRAW);
}

void LineRenderer::Render(Shader& shader, Camera& camera)
{
	shader.Activate();
	camera.Update(shader);
	m_VAO.Bind();
	glLineWidth(4.0f);
	glDrawArrays(GL_LINE_STRIP, 0, m_Vertices.size() / 6);
}
