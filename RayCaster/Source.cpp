#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "ShaderProgram.h"
#include "PrimitiveDrawer.h"

GLFWwindow* window;
int viewport_width = 1280;
int viewport_height = 720;
float t = 0, dt = 0;
float player_speed = 100.f;

glm::vec2 playerPosition = glm::vec2(300, 300);

int map_width = 8, map_height = 8;
float tile_size = 75;
int map[] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
};

void error_callback(int error, const char* description);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

int main() {
	printf("Program starting\n");
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwSetErrorCallback(error_callback);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(viewport_width, viewport_height, "Ray casting engine", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create glfw window");
		glfwTerminate();
		return -1;
	}
	printf("Window created\n");
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // vsync off
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to load glad");
		return -1;
	}
	printf("Glad loaded\n");

	ShaderProgram shaderProgram = ShaderProgram("vert.glsl", "frag.glsl");
	PrimitiveDrawer drawer = PrimitiveDrawer();

	float lastFrameTime = 0;
	float lastTitleUpdate = 0; 
	printf("Loop start\n");
	// render loop
	while (!glfwWindowShouldClose(window)) {
		// time
		t = glfwGetTime();
		dt = t- lastFrameTime;
		lastFrameTime = t;

		// input
		processInput(window);

		// rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.Use();
		drawer.setSize(viewport_width, viewport_height);
		
		// draw player
		drawer.drawPoint(playerPosition.x, playerPosition.y, glm::vec3(1, 1, 0), 10, shaderProgram);
		// draw map
		drawer.fillRect(0, 0, tile_size * map_width, tile_size * map_height, glm::vec3(0, 0, 0), shaderProgram);
		for (int i = 0; i < map_height; i++) {
			for (int j = 0; j < map_width; j++) {
				if(map[i * map_width + j] == 1)
					drawer.fillRect(
						j * tile_size, i * tile_size,
						tile_size, tile_size,
						glm::vec3(1, 1, 1), shaderProgram
					);
			}
		}


		// events/buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		// window name
		if (t - lastTitleUpdate >= 1.f) {
			std::string title = std::to_string((int)(1 / dt)) + std::string(" fps - ") + std::to_string((int)(dt * 1000.f))
				+ std::string(" ms");
			glfwSetWindowTitle(window, title.c_str());
			lastTitleUpdate = t;
		}
		
	}
	printf("Loop end\n");


	glfwTerminate();
	printf("Program exiting\n");
	return 0;
}

void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	viewport_width = width;
	viewport_height = height;
	glViewport(0, 0, viewport_width, viewport_height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	glm::vec2 offset = glm::vec2(0, 0);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) offset.y -= player_speed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) offset.y += player_speed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) offset.x -= player_speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) offset.x = player_speed;
	playerPosition += offset * dt;
}
