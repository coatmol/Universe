#include "Body.h"

Body::Body(glm::vec3 pos, glm::vec3 vel, float mass, float radius, glm::vec3 color)
	: Position(pos),
		Velocity(vel),
		Mass(mass),
		Radius(radius),
		Color(color)
{
	Update(0);
	GenerateVertices();

	m_VAO = VAO();
	m_VAO.Bind();

	m_VBO = new VBO(m_Vertices.data(), GLsizeiptr(m_Vertices.size() * sizeof(GLfloat)));
	m_EBO = new EBO(m_Indices.data(), GLsizeiptr(m_Indices.size() * sizeof(GLuint)));

	m_VAO.LinkAttrib(*m_VBO, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	m_VAO.LinkAttrib(*m_VBO, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	m_VAO.Unbind();
	m_VBO->Unbind();
	m_EBO->Unbind();
}

void Body::Accelerate(const glm::vec3& force)
{
	glm::vec3 acceleration = force / Mass;
	Velocity += acceleration;
}

void Body::Update(float SIM_SPEED)
{
	Position += Velocity * SIM_SPEED;
	m_ModelMatrix = glm::mat4(1.0f);
	m_ModelMatrix = glm::translate(m_ModelMatrix, Position);
}

glm::vec3 Body::GetForce(Body& other)  
{  
	const double G = (6.67430 * 1e-11); // Universal gravitation constant
   glm::vec3 direction = glm::normalize(other.Position - Position);
   float magnitude = static_cast<float>(G * ((Mass * other.Mass) / pow(glm::distance(Position, other.Position), 2)));
   return direction * magnitude;
}

void Body::Render(Shader& shader, Camera& camera)
{
	shader.Activate();
	camera.Update(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader.ProgramID, "model"), 1, GL_FALSE, glm::value_ptr(m_ModelMatrix));
	m_VAO.Bind();
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, nullptr);
}

void Body::GenerateVertices()
{
    m_Vertices.clear();
    m_Indices.clear();
	int stacks = 10;
	int sectors = 10;

	// generate circumference points using integer steps
	for (float i = 0.0f; i < stacks; ++i) {
		float theta1 = (i / stacks) * glm::pi<float>();
		float theta2 = (i + 1) / stacks * glm::pi<float>();
		for (float j = 0.0f; j < sectors; ++j) {
			float phi1 = j / sectors * 2 * glm::pi<float>();
			float phi2 = (j + 1) / sectors * 2 * glm::pi<float>();
			glm::vec3 v1 = sphericalToCartesian(Radius, theta1, phi1);
			glm::vec3 v2 = sphericalToCartesian(Radius, theta1, phi2);
			glm::vec3 v3 = sphericalToCartesian(Radius, theta2, phi1);
			glm::vec3 v4 = sphericalToCartesian(Radius, theta2, phi2);

			uint32_t base = static_cast<uint32_t>(m_Vertices.size() / 6);

			// Triangle 1: v1-v2-v3
			m_Vertices.insert(m_Vertices.end(), { v1.x, v1.y, v1.z, Color.r, Color.g, Color.b }); //      /|
			m_Vertices.insert(m_Vertices.end(), { v2.x, v2.y, v2.z, Color.r, Color.g, Color.b }); //     / |
			m_Vertices.insert(m_Vertices.end(), { v3.x, v3.y, v3.z, Color.r, Color.g, Color.b }); //    /__|

			// Triangle 2: v2-v4-v3
			m_Vertices.insert(m_Vertices.end(), { v2.x, v2.y, v2.z, Color.r, Color.g, Color.b });
			m_Vertices.insert(m_Vertices.end(), { v4.x, v4.y, v4.z, Color.r, Color.g, Color.b });
			m_Vertices.insert(m_Vertices.end(), { v3.x, v3.y, v3.z, Color.r, Color.g, Color.b });

			m_Indices.insert(m_Indices.end(), {
				base,     base + 1, base + 2,   // tri 1
				base + 3, base + 4, base + 5    // tri 2
			});
		}
	}
}



void Body::Destroy()
{
	m_VAO.Delete();
	m_VBO->Delete();
	m_EBO->Delete();
	delete m_VBO;
	delete m_EBO;
}