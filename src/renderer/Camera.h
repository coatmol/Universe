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
	glm::mat4 CameraMatrix = glm::mat4(1.0f);

	int width, height;
	float FOVdeg, nearPlane, farPlane;

	void UpdateMatrix();
	void Update(Shader& shader);
	void Update(Shader& shader, const char* uniform, glm::mat4 matrix);
	void HandleInput(GLFWwindow* window, float dt);
	void HandleScroll(GLFWwindow* window, double xoffset, double yoffset);

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
private:
	float speed = 5.0f;
	float sensitivity = 100.0f;

	bool firstClick = true;
};