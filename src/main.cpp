#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "renderer/Shader.h"
#include "renderer/gl/VAO.h"
#include "renderer/gl/VBO.h"
#include "renderer/gl/EBO.h"
#include "renderer/Camera.h"

#include "engine/Body.h"
#include "engine/Skybox.h"

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

	glfwSwapInterval(1); // Enable vsync
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	Camera camera(WIDTH, HEIGHT, glm::vec3(1500.0f, 0.0f, 100.0f), 80.0f, 0.1f, 500000.0f);
	Shader shader("assets/shaders/default-vert.glsl", "assets/shaders/default-frag.glsl");
	Shader lightShader("assets/shaders/light-vert.glsl", "assets/shaders/light-frag.glsl");
	Shader skyboxShader("assets/shaders/skybox-vert.glsl", "assets/shaders/skybox-frag.glsl");

	std::vector<std::string> faces = 
	{
		"assets/textures/skybox_right.png",
		"assets/textures/skybox_left.png",
		"assets/textures/skybox_up.png",
		"assets/textures/skybox_down.png",
		"assets/textures/skybox_front.png",
		"assets/textures/skybox_back.png"
	};

	const float SCALE = 0.0001f;
	const float SIM_SPEED = 10000;

	std::vector<Body*> bodies = {
		// POSITION, VELOCITY, MASS, RADIUS, COLOR
		//SUN
		new Body(glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			333000,
			696340 * SCALE,
			glm::vec3(1.0f, 0.0f, 0.0f)),
		//EARTH
		new Body(glm::vec3(15090000 * SCALE, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			100,
			63780 * SCALE,
			glm::vec3(0.0f, 0.0f, 1.0f)),
		//MOON
		new Body(glm::vec3((15090000 + 38440) * SCALE, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, -0.001834f * SCALE),
			1,
			17370 * SCALE,
			glm::vec3(1.0f, 1.0f, 1.0f))
	};

	Skybox skybox(faces);

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

		//skybox.Render(skyboxShader, camera);

		for each(Body* body in bodies)
		{
			for each(Body* other in bodies)
			{
				if (body == other)
					continue;
				glm::vec3 force = body->GetForce(*other);
				body->Accelerate(force);
			}

			body->Update(SIM_SPEED);
			body->Render(shader, camera);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for each(Body* body in bodies)
	{
		body->Destroy();
	}
	shader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

