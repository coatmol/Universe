#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "Shader.h"

class Camera
{
public:
	Camera(int width, int height, glm::vec3 pos, float FOV, float np, float fp);

	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	int width, height;
	float FOVdeg, nearPlane, farPlane;

	void Update(Shader& shader, const char* uniform);
	void HandleInput(GLFWwindow* window);

private:
	float speed = 0.05f;
	float sensitivity = 0.1f;

	bool firstClick = true;
};