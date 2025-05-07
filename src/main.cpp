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

	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 10.0f), 80.0f, 0.1f, 500000.0f);
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
	const float MASS_SCALE = 1e24f;
	const float DIST_SCALE = 1e6f;
	const float SIM_SPEED = 10000000;

	float sunMass = 1.989e30f / MASS_SCALE;

	const double G = 6.67430e-11;
	double   r_phys = 150.96e9;
	double v_phys = sqrt(G * sunMass / r_phys);

	float earthMass = 5.972e24f / MASS_SCALE;
	float earthDist = 150.96e6f / DIST_SCALE;
	float v_sim_per_s = static_cast<float>(v_phys / DIST_SCALE);

	float moonMass = 7.34767309f / MASS_SCALE;
	float moonDist = (384400.f / DIST_SCALE) + earthDist;

	std::vector<Body*> bodies = {
		// POSITION, VELOCITY, MASS, RADIUS, COLOR
		//SUN
		new Body(glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 0.0f),
			sunMass,
			10.9f,
			glm::vec3(1.0f, 0.0f, 0.0f),
			true),
		//EARTH
		new Body(glm::vec3(earthDist, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, -v_sim_per_s),
			earthMass,
			0.1f,
			glm::vec3(0.0f, 0.0f, 1.0f)),
		//MOON
		/*new Body(glm::vec3(moonDist, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, -0.001834f * SCALE),
			moonMass,
			0.27f,
			glm::vec3(1.0f, 1.0f, 1.0f))*/
	};

	Skybox skybox(faces);

	glfwSetWindowUserPointer(window, &camera);
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		if(camera)
			camera->HandleScroll(window, xoffset, yoffset);
	});

	double currentFrame = glfwGetTime();
	double lastFrame = currentFrame;
	double deltaTime = 0;

	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		std::cout << "dt: " << deltaTime << std::endl;

		camera.HandleInput(window, deltaTime);

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
				body->Accelerate(force, SIM_SPEED * deltaTime);
			}

			body->Update(SIM_SPEED * deltaTime);
			body->Render(body->Glows ? lightShader : shader, camera);
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

