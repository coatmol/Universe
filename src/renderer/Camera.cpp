#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 pos, float FOV, float np, float fp)
	: Position(pos), width(width), height(height), FOVdeg(FOV), nearPlane(np), farPlane(fp)
{
}

void Camera::UpdateMatrix()
{
	glm::mat4 view = GetViewMatrix();
	glm::mat4 projection = GetProjectionMatrix();

	CameraMatrix = projection * view;
}

void Camera::Update(Shader& shader)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.ProgramID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(CameraMatrix));
}

void Camera::Update(Shader& shader, const char* uniform, glm::mat4 matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.ProgramID, uniform), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Camera::HandleInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += speed * Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += speed * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.05f;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick)
		{
			glfwSetCursorPos(window, width / 2, height / 2);
			firstClick = false;
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		if (!((glm::angle(newOrientation, Up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -Up) <= glm::radians(5.0f))))
		{
			Orientation = newOrientation;
		}

		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
}

void Camera::HandleScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	FOVdeg = std::max(1.0f, std::min(80.0f, FOVdeg - (float)yoffset));
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Orientation, Up);
}

glm::mat4 Camera::GetProjectionMatrix()
{
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	return glm::perspective(glm::radians(FOVdeg), aspect, nearPlane, farPlane);;
}
