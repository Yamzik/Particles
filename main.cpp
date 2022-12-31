#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include <Windows.h>
#include <Shader.h>
#include <Texture.h>
#include <ComputeShader.h>
#include <GpuParticleResolver.h>
#include <ParticleResolver.h>
#include <ShaderSources.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Particle.h"
#include <vector>

#define NAMEOF(name) #name

void framebuffer_size_callback(GLFWwindow* window, int _width, int _height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

glm::vec2 resolution;
glm::vec2 mouse_pos;
bool mouse_click;
float mix_val = 0.2f;

// timing 
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f; // time of last frame

int main()
{
	srand((unsigned)time(NULL));

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwInit();

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	resolution.x = mode->width;
	resolution.y = mode->height;

	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow((int)resolution.x, (int)resolution.y, "Particles", monitor, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, (int)resolution.x, (int)resolution.y);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// don't forget to activate/use the shader before setting uniforms!
	Shader ourShader("glsl/ParticleVertex.glsl", "glsl/ParticleFragment.glsl");
	//Shader ourShader(src::particleVertex, src::particleFragment, 1);
	ourShader.setVec2f("resolution", resolution.x, resolution.y);
	ComputeShader computeShader("glsl/TestCompute.glsl");
	//ComputeShader computeShader(src::particleCompute, 1);

	pt::setup_diameters(4, 4, 4, 4);
	pt::setup_masses(1, 1, 1, 1);

	const int groups_num = 4;
	ParticleGroup particle_groups[groups_num] =
	{
		ParticleGroup(1000, White, &ourShader, &resolution),
		ParticleGroup(1000, Green, &ourShader, &resolution),
		ParticleGroup(1000, Red,   &ourShader, &resolution),
		ParticleGroup(1000, Blue,  &ourShader, &resolution),
	};

	//const int NUM_INVOCATIONS = 20;
	//const int WORKGROUP_SIZE = 1;

	//struct comp {
	//	float x;
	//	float y;
	//};

	//struct comp* default_values = new struct comp[NUM_INVOCATIONS];
	//Particle* default_particles = new Particle[NUM_INVOCATIONS];

	//// std::fill(default_values, default_values + NUM_INVOCATIONS, -1.0);
	//unsigned int float_buffer, particle_buffer;

	//glGenBuffers(1, &float_buffer);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, float_buffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER,
	//	NUM_INVOCATIONS * sizeof(struct comp),
	//	default_values,
	//	GL_DYNAMIC_COPY);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, float_buffer);

	//computeShader.use();
	//glDispatchCompute(NUM_INVOCATIONS / WORKGROUP_SIZE, 1, 1);
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//// Now map the buffer so that we can check its values
	//struct comp* float_data = (struct comp*)glMapNamedBuffer(float_buffer,
	//	GL_READ_WRITE);
	//std::vector<struct comp> float_buffer_data(NUM_INVOCATIONS);
	//for (int i = 0; i < NUM_INVOCATIONS; i++) {
	//	float_buffer_data[i] = float_data[i];
	//}

	//glUnmapNamedBuffer(float_buffer);

	const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
	const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model

	std::cout << vendor << std::endl;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int fCounter = 0;
	res::GpuParticleResolver resolver(particle_groups, groups_num, &computeShader,
		&resolution, &mouse_pos, &mouse_click);
	//res::ParticleResolver resolver(particle_groups, groups_num);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (fCounter > 20) {
			std::cout << "FPS: " << 1 / deltaTime << std::endl;
			fCounter = 0;
		}
		else {
			fCounter++;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		deltaTime = 0.016f;
		resolver.resolve();
		for (int i = 0; i < groups_num; i++)
		{
			particle_groups[i].render(true);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int _width, int _height)
{
	resolution.x = _width;
	resolution.y = _height;
	glViewport(0, 0, _width, _height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		mix_val += mix_val <= 0.9f ? 0.1f : 0;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		mix_val -= mix_val >= 0.1f ? 0.1f : 0;
}

void mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouse_pos.x = xpos;
	mouse_pos.y = -ypos + resolution.y;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		mouse_click = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		mouse_click = false;
}
