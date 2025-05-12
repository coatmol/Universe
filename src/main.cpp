#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

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

	bool toolActive = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	Camera camera(WIDTH, HEIGHT, glm::vec3(600.0f, 0.0f, 150.0f), 80.0f, 0.1f, 500000.0f);
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


	float SIM_SPEED = 0.00001f;

	std::vector<Body*> bodies = {};
	int selectedBody = -1;
	int lightBody = 0;

	glm::vec3 ambientLight = glm::vec3();
	glUniform3fv(glGetUniformLocation(shader.ProgramID, "uAmbientLight"), 1, glm::value_ptr(ambientLight));

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

		camera.HandleInput(window, deltaTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 255.0f);
		camera.UpdateMatrix();

		if (bodies.size() != 0 && lightBody < bodies.size()) {
			glUniform3fv(glGetUniformLocation(shader.ProgramID, "uLightPos"), 1, glm::value_ptr(bodies[lightBody]->Position));
			glUniform3fv(glGetUniformLocation(shader.ProgramID, "uLightColor"), 1, glm::value_ptr(bodies[lightBody]->Color));
		}
		else {
			glUniform3fv(glGetUniformLocation(shader.ProgramID, "uLightPos"), 1, glm::value_ptr(glm::vec3()));
			glUniform3fv(glGetUniformLocation(shader.ProgramID, "uLightColor"), 1, glm::value_ptr(glm::vec3(1,1,1)));
		}

		//skybox.Render(skyboxShader, camera);

		for each(Body* body in bodies)
		{
			for each(Body * other in bodies)
			{
				if (body == other)
					continue;
				glm::vec3 force = body->GetForce(*other);
				body->Accelerate(force, SIM_SPEED);
			}

			body->Update(SIM_SPEED);
			body->Render(body->Glows ? lightShader : shader, camera);
		}

#pragma region ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		ImGui::Begin("Tools", &toolActive, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Presets"))
			{
				if (ImGui::MenuItem("Solar System"))
					bodies = {
						// POSITION, VELOCITY, MASS, RADIUS, COLOR
						//SUN
						new Body(glm::vec3(0.0f, 0.0f, 0.0f),
							glm::vec3(0.0f, 0.0f, 0.0f),
							1.989e25,
							1414,
							glm::vec3(1.0f, 0.0f, 0.0f),
							true),
							//mars
							new Body(glm::vec3(-3000.0f, 650.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 500.0f),
								5.97219e23,
								5515,
								glm::vec3(1.0f, 0.25f, 0.56f)),
								//earth
								new Body(glm::vec3(5000.0f, 650.0f, 0.0f),
									glm::vec3(0.0f, 0.0f, -500.0f),
									5.97219e23,
									5515,
									glm::vec3(0.0f, 1.0f, 1.0f)),
									//moon
									new Body(glm::vec3(5250.0f, 650.0f, 0.0f),
										glm::vec3(0.0f, 0.0f, -50.0f),
										5.97219e21,
										5515,
										glm::vec3(1.0f, 1.0f, 1.0f)),

										//Jupiter
										new Body(glm::vec3(0.0f, 500.0f, 9000.0f),
											glm::vec3(-500.0f, 50.0f, 0.0f),
											5.97219 * pow(10, 23.5),
											5515,
											glm::vec3(1.0f, 0.5f, 0.15f)),
										new Body(glm::vec3(0.0f, 550.0f, 9500.0f),
											glm::vec3(0.0f, 0.0f, -50.0f),
											5.97219e21,
											5515,
											glm::vec3(1.0f, 1.0f, 1.0f)),
										new Body(glm::vec3(0.0f, 450.0f, 8500.0f),
											glm::vec3(0.0f, 0.0f, -50.0f),
											5.97219e21,
											5515,
											glm::vec3(1.0f, 1.0f, 1.0f)),
										new Body(glm::vec3(100.0f, 500.0f, 9000.0f),
											glm::vec3(50.0f, 0.0f, 0.0f),
											5.97219e21,
											5515,
											glm::vec3(1.0f, 1.0f, 1.0f)),

											//Neptune
											new Body(glm::vec3(0.0f, -500.0f, -10500.0f),
												glm::vec3(-350.0f, 50.0f, 0.0f),
												5.97219 * pow(10, 23.5),
												5515,
												glm::vec3(0.35f, 0.5f, 0.15f)),
											new Body(glm::vec3(350.0f, -450.0f, -10500.0f),
												glm::vec3(0.0f, 0.0f, -550.0f),
												5.97219e21,
												5515,
												glm::vec3(1.0f, 1.0f, 1.0f)),
											new Body(glm::vec3(-350.0f, 450.0f, -10500.0f),
												glm::vec3(0.0f, 0.0f, -550.0f),
												5.97219e21,
												5515,
												glm::vec3(1.0f, 1.0f, 1.0f)),
											new Body(glm::vec3(0.0f, -450.0f, -10500.0f),
												glm::vec3(-550.0f, 0.0f, 0.0f),
												5.97219e21,
												5515,
												glm::vec3(1.0f, 1.0f, 1.0f)),
					};
				if (ImGui::MenuItem("Empty"))
					bodies = {};
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("General Options");
		ImGui::InputFloat("SimulationSpeed", &SIM_SPEED);
		ImGui::SliderFloat3("Camera Position", glm::value_ptr(camera.Position), -1e3, 1e3);
		ImGui::Separator();

		ImGui::Text("Lighting Options");
		ImGui::InputInt("Main Light Body ID", &lightBody, 1, 2);
		if(ImGui::ColorEdit3("Ambient light color", glm::value_ptr(ambientLight)))
			glUniform3fv(glGetUniformLocation(shader.ProgramID, "uAmbientLight"), 1, glm::value_ptr(ambientLight));

		if (selectedBody == -1 || selectedBody > bodies.size())
		{
			ImGui::Separator();
			if (ImGui::Button("New Body"))
				bodies.push_back(new Body(camera.Position + camera.Orientation * 50.0f, glm::vec3(), 1e20, 1411));
			ImGui::Separator();
			ImGui::BeginChild("Scrolling");
			for (int i = 0; i < bodies.size(); i++)
			{
				std::string buttonLabel = "Body #" + std::to_string(i);
				if (ImGui::Button(buttonLabel.c_str()))
					selectedBody = i;
			}
			ImGui::EndChild();
		}
		else
		{
			ImGui::Separator();
			ImGui::Text("Body Properties");
			ImGui::Text("Body #%d", selectedBody);

			ImGui::InputFloat3("Position", glm::value_ptr(bodies[selectedBody]->Position));
			ImGui::InputFloat3("Velocity", glm::value_ptr(bodies[selectedBody]->Velocity));
			if (ImGui::ColorEdit3("Color", glm::value_ptr(bodies[selectedBody]->Color)))
				bodies[selectedBody]->RefreshMesh();

			ImGui::InputDouble("Mass", &bodies[selectedBody]->Mass);
			if (ImGui::InputFloat("Density", &bodies[selectedBody]->Density, 1, 10))
				bodies[selectedBody]->RefreshMesh();
			ImGui::Checkbox("Glows", &bodies[selectedBody]->Glows);

			ImGui::Separator();

			if (ImGui::Button("Refresh Mesh"))
				bodies[selectedBody]->RefreshMesh();
			if (ImGui::Button("Look at"))
				camera.LookAt(bodies[selectedBody]->Position);
			if (ImGui::Button("Go to"))
				camera.Position = bodies[selectedBody]->Position + bodies[selectedBody]->Radius;
			if (ImGui::Button("Deselect"))
				selectedBody = -1;
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for each(Body* body in bodies)
	{
		body->Destroy();
	}
	shader.Delete();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

