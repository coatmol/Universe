#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../src/renderer/Shader.h"
#include "../src/renderer/Camera.h"
#include "../src/renderer/gl/VAO.h"
#include "../src/renderer/gl/VBO.h"
#include "../src/renderer/gl/EBO.h"
#include "../src/utils/Math.h"

// Physics body
class Body
{
public:
	Body(glm::vec3 pos, glm::vec3 vel, float mass, float radius, glm::vec3 color=glm::vec3(1,1,1), bool glows=false);

	void Accelerate(const glm::vec3& force, float SIM_SPEED);
	void Render(Shader& shader, Camera& camera);
	glm::vec3 GetForce(Body& other);
	void Update(float SIM_SPEED);
	void Destroy();

	bool operator==(const Body& other) const
	{
		return Position == other.Position &&
			Velocity == other.Velocity &&
			Mass == other.Mass &&
			Radius == other.Radius;
	}

	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec3 Velocity;
	float Mass, Radius;
	bool Glows;
private:
	glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
	VAO m_VAO;
	VBO* m_VBO;
	EBO* m_EBO;

	std::vector<GLfloat> m_Vertices;
	std::vector<GLuint> m_Indices;

	void GenerateVertices();
};