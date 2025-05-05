#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer/Shader.h"
#include "renderer/gl/VAO.h"
#include "renderer/gl/VBO.h"
#include "renderer/gl/EBO.h"
#include "renderer/Camera.h"

#include "engine/Body.h"

int WIDTH = 1366;
int HEIGHT = 720;

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Universe", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f), 80.0f, 0.1f, 1000.0f);
	Shader shader("assets/shaders/default-vert.glsl", "assets/shaders/default-frag.glsl");
	Shader lightShader("assets/shaders/light-vert.glsl", "assets/shaders/light-frag.glsl");

	Body body(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Body body1(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));

	glfwSetWindowUserPointer(window, &camera);
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		if(camera)
			camera->HandleScroll(window, xoffset, yoffset);
	});

	while (!glfwWindowShouldClose(window))
	{
		camera.HandleInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 255.0f);
		camera.UpdateMatrix();

		body.Update();
		body.Render(shader, camera);

		body1.Update();
		body1.Render(shader, camera);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	body.Destroy();
	shader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

