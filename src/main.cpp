#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer/Shader.h"
#include "renderer/gl/VAO.h"
#include "renderer/gl/VBO.h"
#include "renderer/gl/EBO.h"

int main()
{
	int WIDTH = 1366;
	int HEIGHT = 720;

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

	GLfloat vertices[] =
	{
		// ============ VERTEX POSITIONS ============== || ============ VERTEX COLORS ============== \\ 
		-0.5f,		-0.5f * float(sqrt(3)) / 3,		0.0f,	0.8f, 0.3f, 0.02f,	// Lower left corner
		0.5f,		-0.5f * float(sqrt(3)) / 3,		0.0f,	0.8f, 0.3f, 0.02f,	// Lower right corner
		0.0f,		0.5f * float(sqrt(3)) * 2 / 3,	0.0f,	1.0f, 0.6f, 0.32f,	// Upper corner
		-0.5f / 2,	0.5f * float(sqrt(3)) / 6,		0.0f,	0.9f, 0.45f, 0.17f,	// Inner left
		0.5f / 2,	0.5f * float(sqrt(3)) / 6,		0.0f,	0.9f, 0.45f, 0.17f,	// Inner right
		0.0f,		-0.5f * float(sqrt(3)) / 3,		0.0f,	0.8f, 0.3f, 0.02f	// Inner down
	};

	GLuint indices[] =
	{
		0, 3, 5, // Lower left triangle
		3, 2, 4, // Upper triangle
		5, 4, 1 // Lower right triangle
	};

	Shader shader("assets/shaders/default-vert.glsl", "assets/shaders/default-frag.glsl");

	VAO vao;
	vao.Bind();

	VBO vbo(vertices, sizeof(vertices));
	EBO ebo(indices, sizeof(indices));

	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 255.0f);

		shader.Activate();
		vao.Bind();
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	vao.Delete();
	vbo.Delete();
	ebo.Delete();
	shader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}