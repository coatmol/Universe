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
#include "engine/Grid.h"
#include "renderer/LineRenderer.h"

struct Snapshot {
	glm::vec3 pos, vel, color;
	double mass;
};

inline glm::vec3 computeForce(Snapshot a, Snapshot b)
{
	const double G = 6.67430e-11; // Universal gravitation constant
	glm::vec3 direction = glm::normalize(b.pos - a.pos);
	float magnitude = static_cast<float>(G * ((a.mass * b.mass) / pow(glm::distance(a.pos, b.pos), 2)));
	return direction * magnitude;
}

int WIDTH = 1366;
int HEIGHT = 720;
static bool f11PressedLastFrame = false;

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
	glfwWindowHint(GLFW_SAMPLES, 4);

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
	glEnable(GL_MULTISAMPLE);
	glFrontFace(GL_CCW);

	bool toolActive = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 0.0f), 80.0f, 0.1f, 500000.0f);
	Shader shader("assets/shaders/default-vert.glsl", "assets/shaders/default-frag.glsl");
	Shader lightShader("assets/shaders/light-vert.glsl", "assets/shaders/light-frag.glsl");
	Shader skyboxShader("assets/shaders/skybox-vert.glsl", "assets/shaders/skybox-frag.glsl");
	Shader debugShader("assets/shaders/debug-vert.glsl", "assets/shaders/debug-frag.glsl");

	std::vector<Snapshot> snaps;
	std::vector<std::vector<GLfloat>> trajectoryVerts = { {0,0,0, 0,100,0}, {0,0,0,0,0,100} };
	LineRenderer trajectoryLine(trajectoryVerts[0]);
	int trajectorySize = 100;

	std::vector<std::string> faces = 
	{
		"assets/textures/skybox_right.png",
		"assets/textures/skybox_left.png",
		"assets/textures/skybox_top.png",
		"assets/textures/skybox_bottom.png",
		"assets/textures/skybox_front.png",
		"assets/textures/skybox_back.png"
	};

	float SIM_SPEED = 1;
	bool SHOW_GRID = true;
	bool GRID_FOLLOWS_CAMERA = true;
	glm::vec3 bodyCameraOffset = glm::vec3();
	float GRID_SIZE = 20000;
	int GRID_DIVS = 100;
	bool SHOW_SKYBOX = true;
	bool SHOW_TRAJECTORIES = true;

	std::vector<Body*> bodies = {};
	int selectedBody = -1;
	int lightBody = 0;
	int trackingBody = -1;
	int followingBody = -1;

	glm::vec3 ambientLight = glm::vec3();
	shader.Activate();
	glUniform3fv(glGetUniformLocation(shader.ProgramID, "uAmbientLight"), 1, glm::value_ptr(ambientLight));
	auto locLightPos = glGetUniformLocation(shader.ProgramID, "uLightPos");
	auto locLightColor = glGetUniformLocation(shader.ProgramID, "uLightColor");

	Skybox skybox(faces);
	Grid grid(GRID_SIZE, GRID_DIVS);

	glfwSetWindowUserPointer(window, &camera);
	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
	{
		Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		if(camera)
			camera->HandleScroll(window, xoffset, yoffset);
	});
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
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
		if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && !f11PressedLastFrame) {
			if (glfwGetWindowMonitor(window) == nullptr) {
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			}
			else {
				glfwSetWindowMonitor(window, nullptr, 100, 100, WIDTH, HEIGHT, 0);
			}
		}
		f11PressedLastFrame = glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 255.0f);
		camera.UpdateMatrix();

		if (selectedBody > bodies.size())
			selectedBody = -1;

		shader.Activate();
		if (lightBody >= 0 && !bodies.empty() && lightBody < bodies.size()) {
			glUniform3fv(locLightPos, 1, glm::value_ptr(bodies[lightBody]->Position));
			glUniform3fv(locLightColor, 1, glm::value_ptr(bodies[lightBody]->Color));
		}
		else {
			glUniform3fv(locLightPos, 1, glm::value_ptr(glm::vec3()));
			glUniform3fv(locLightColor, 1, glm::value_ptr(glm::vec3(1,1,1)));
		}

		if(SHOW_SKYBOX)
			skybox.Render(skyboxShader, camera);

		for (Body* body : bodies)
		{
			for (Body* other : bodies)
			{
				if (body == other || body->Mass == 0)
					continue;
				glm::vec3 force = body->GetForce(*other);
				body->Accelerate(force, (SIM_SPEED * deltaTime) / 10000);
			}

			body->Update((SIM_SPEED* deltaTime) / 10000);
			body->Render(body->Glows ? lightShader : shader, camera);
		}

		if (SHOW_GRID)
		{
			grid.Update(bodies, GRID_FOLLOWS_CAMERA ? glm::floor(camera.Position / (GRID_SIZE / GRID_DIVS)) * (GRID_SIZE / GRID_DIVS) : glm::vec3());
			grid.Render(debugShader, camera);
		}


		if (trackingBody >= 0 && !bodies.empty() && trackingBody < bodies.size())
			camera.LookAt(bodies[trackingBody]->Position);
		else
			trackingBody = -1;
		
		if (followingBody >= 0 && !bodies.empty() && followingBody < bodies.size())
			camera.Position = bodies[followingBody]->Position - bodyCameraOffset;
		else
			followingBody = -1;

#pragma region trajectory
		if (SHOW_TRAJECTORIES)
		{
			snaps.clear();
			trajectoryVerts.clear();
			trajectoryVerts.assign(bodies.size(), std::vector<GLfloat>());
			for (auto& v : trajectoryVerts)
				v.reserve(trajectorySize * 3);
			for (auto& b : bodies)
				snaps.push_back({ b->Position, b->Velocity * ((SIM_SPEED < 0) ? -1.0f : 1.0f), b->Color, b->Mass });

			for (int step = 0; step < trajectorySize; ++step) {
				// Compute forces on snaps[i] from snaps[j]
				std::vector<glm::vec3> acc(snaps.size());
				for (int i = 0; i < snaps.size(); ++i) {
					glm::vec3 f{ 0 };
					for (int j = 0; j < snaps.size(); ++j) if (i != j)
						f += computeForce(snaps[i], snaps[j]);
					acc[i] = f / static_cast<float>(snaps[i].mass);
				}
				// Integrate (e.g. Verlet or RK4) on snaps[*]
				for (int i = 0; i < snaps.size(); ++i) {
					// Example: simple semi-implicit Euler
					snaps[i].vel += acc[i] * (float)deltaTime;
					snaps[i].pos += snaps[i].vel * (float)deltaTime;
					// Store for rendering
					trajectoryVerts[i].push_back(snaps[i].pos.x);
					trajectoryVerts[i].push_back(snaps[i].pos.y);
					trajectoryVerts[i].push_back(snaps[i].pos.z);
					trajectoryVerts[i].push_back(snaps[i].color.r);
					trajectoryVerts[i].push_back(snaps[i].color.g);
					trajectoryVerts[i].push_back(snaps[i].color.b);
				}
			}
			for (auto& verts : trajectoryVerts)
			{
				trajectoryLine.Update(verts);
				trajectoryLine.Render(debugShader, camera);
			}
		}
#pragma endregion

#pragma region ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		ImGui::Begin("Tools", &toolActive, ImGuiWindowFlags_MenuBar);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Presets"))
			{
				if (ImGui::MenuItem("Solar System")) {
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
					selectedBody = -1;
				}
				if (ImGui::MenuItem("Stable Orbit")) {
					bodies = {
						new Body(glm::vec3(), glm::vec3(), 1e20, 1, glm::vec3(1,1,1), true),
						new Body(glm::vec3(1000, 0, 0), glm::vec3(0, 0, -3000), 1e18, 1, glm::vec3(0,0,1))
					};
					selectedBody = -1;
				}
				if (ImGui::MenuItem("Dynamic Orbit")) {
					bodies = {
						new Body(glm::vec3(), glm::vec3(0, 1000, 0), 1e20, 1, glm::vec3(1,1,1), true),
						new Body(glm::vec3(1000, 0, 0), glm::vec3(0, 0, -3000), 1e18, 1, glm::vec3(0,0,1))
					};
					selectedBody = -1;
				}
				if (ImGui::MenuItem("Spinny Orbit")) {
					bodies = {
						new Body(glm::vec3(), glm::vec3(0, 1000, 0), 1e20, 1411, glm::vec3(1,1,1), true),
						new Body(glm::vec3(200, 200, -200), glm::vec3(0, 0, 7500), 1e20, 1411, glm::vec3(0,0,1))
					};
					selectedBody = -1;
				}
				if (ImGui::MenuItem("Blackhole orbit")) {
					bodies = {
						new Body(glm::vec3(), glm::vec3(0, 10000, 0), 1e22, 3000, glm::vec3(1,1,1), true),
						new Body(glm::vec3(0, 250, 2500), glm::vec3(0, -10000, 0), 1e22, 3000, glm::vec3(1,1,1), true)
					};
					selectedBody = -1;
				}
				if (ImGui::MenuItem("Empty")) {
					bodies = {};
					selectedBody = -1;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("General Options");
		ImGui::InputFloat("Simulation Speed", &SIM_SPEED);
		ImGui::InputFloat3("Camera Position", glm::value_ptr(camera.Position));
		ImGui::Checkbox("Show Grid", &SHOW_GRID);
		ImGui::Checkbox("Grid Follows Camera", &GRID_FOLLOWS_CAMERA);
		if (ImGui::InputFloat("Grid Size", &GRID_SIZE, 10, 100))
			grid.Update(GRID_SIZE, GRID_DIVS);
		if (ImGui::InputInt("Grid Divisions", &GRID_DIVS, 5, 10))
			grid.Update(GRID_SIZE, GRID_DIVS);
		ImGui::Checkbox("Show Skybox", &SHOW_SKYBOX);
		ImGui::Checkbox("Show Trajectories", &SHOW_TRAJECTORIES);
		ImGui::InputInt("Trajectory Size", &trajectorySize);
		ImGui::Separator();

		ImGui::Text("Lighting Options");
		ImGui::InputInt("Main Light Body ID", &lightBody, 1, 2);
		if (ImGui::ColorEdit3("Ambient light color", glm::value_ptr(ambientLight)))
		{
			shader.Activate();
			glUniform3fv(glGetUniformLocation(shader.ProgramID, "uAmbientLight"), 1, glm::value_ptr(ambientLight));
		}

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
			ImGui::SameLine();
			if (ImGui::Button("Track/Untrack"))
				if (trackingBody != selectedBody)
					trackingBody = selectedBody;
				else
					trackingBody = -1;
			ImGui::SameLine();
			if (ImGui::Button("Follow")) {
				if (followingBody != selectedBody) {
					followingBody = selectedBody;
					bodyCameraOffset = bodies[selectedBody]->Position - camera.Position;
				}
				else
					followingBody = -1;
			}

			if (ImGui::Button("Look at"))
				camera.LookAt(bodies[selectedBody]->Position);
			ImGui::SameLine();
			if (ImGui::Button("Go to"))
				camera.Position = bodies[selectedBody]->Position + bodies[selectedBody]->Radius;
			ImGui::SameLine();
			if (ImGui::Button("Deselect"))
				selectedBody = -1;
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
			if (ImGui::Button("Del"))
			{
                bodies.erase(bodies.begin() + selectedBody);
				selectedBody = -1;
			}
			ImGui::PopStyleColor(3);
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#pragma endregion

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for (Body* body : bodies)
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

