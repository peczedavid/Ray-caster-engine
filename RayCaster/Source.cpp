#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "ShaderProgram.h"
#include "PrimitiveDrawer.h"
#include "ScreenBuffer.h"
#include "Texture.h"
#include <math.h>
#include <vector>

float rayScale = 1.0f;
constexpr float fov = 60; // degrees
float PI = 3.14159265359f;
float DEG = 0.0174532925f / rayScale;

int bufferWidth = fov * rayScale, bufferHeight = 0.75f * bufferWidth;
ScreenBuffer screenBuffer;
ShaderProgram drawerProgram;
PrimitiveDrawer drawer;

GLFWwindow* window;
int viewport_width = 1000;
int viewport_height = 0.75f * viewport_width;
constexpr int texture_width = 64;
constexpr int texture_height = 64;

float t = 0, dt = 0;
float player_speed = 1.f;
glm::vec2 playerPosition = glm::vec2(3.5, 5.5);
glm::vec2 playerPositionDelta = glm::vec2(0, 0);
float player_angle = PI/2 + 0.00001f;
float collisionDistance = 0.25f;

int map_width = 8, map_height = 10;
float tile_size = 10;
int mapWalls[] = {
	2, 2, 2, 2, 1, 1, 1, 1,
	2, 0, 2, 0, 0, 1, 0, 1,
	2, 0, 2, 0, 0, 1, 0, 1,
	2, 0, 2, 0, 0, 0, 0, 1,
	2, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
};

void error_callback(int error, const char* description);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

float limitAngle(float rad);

void drawMap2D();
void drawPlayer2D();
void drawRays2D3D();

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
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to load glad");
		return -1;
	}
	printf("Glad loaded\n");

	std::vector<glm::vec2> bufferVertices = {
		glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f), glm::vec2(0.f, 1.f)
	};
	//screenBuffer = ScreenBuffer(bufferWidth, bufferHeight, glm::vec4(0.2f, 0.2f, 1.f, 1.f), bufferVertices);
	screenBuffer = ScreenBuffer(bufferWidth, bufferHeight, glm::vec4(0.2f, 0.2f, 1.f, 1.f));

	//drawerProgram = ShaderProgram("drawer.vert", "drawer.frag");
	//drawer = PrimitiveDrawer();
	//drawer.init();

	std::vector<glm::vec4> checkerBoard;
	checkerBoard.resize(texture_width * texture_height);
	const glm::vec4 white(1.f, 1.f, 1.f, 1.f);
	const glm::vec4 black(0.f, 0.f, 0.f, 1.f);
	for (int x = 0; x < texture_width; x++) {
		for (int y = 0; y < texture_height; y++) {
			checkerBoard[x * texture_width + y] =
				(x & 1) ^ (y ^ 1) ? black : white;
		}
	}

	std::vector<Texture> textures;
	textures.resize(2);

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

		//drawerProgram.Use();
		//drawer.setSize(viewport_width, viewport_height);
		
		// draw map
		//drawMap2D();

		// draw player
		drawRays2D3D();
		//drawPlayer2D();
		/*for (int x = 0; x < 320/2; x++) {
			for (int y = 0; y < 240/2; y++) {
				screenBuffer.setPixel(glm::vec4(1, 0, 0, 1), x, y);
			}
		}*/
		screenBuffer.drawBuffer();

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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	float mul = 3.f / 2.f;
	if (yoffset == -1) mul = 2.f / 3.f;
	//rayScale += 0.5f * yoffset;
	rayScale *= mul;
	rayScale = glm::clamp(rayScale, 0.1f, 20.f);

	DEG = 0.0174532925f / rayScale;
	bufferWidth = fov * rayScale;
	bufferHeight = 0.75f * bufferWidth;

	screenBuffer.resize(bufferWidth, bufferHeight);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player_angle += PI * dt;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player_angle -= PI * dt;

	player_angle = limitAngle(player_angle);
	playerPositionDelta.x = cosf(player_angle);
	playerPositionDelta.y = -sinf(player_angle);

	glm::vec2 nextPosition = playerPosition;
	int iNow = (int)playerPosition.y;
	int jNow = (int)playerPosition.x;
	int iNext = (int)nextPosition.y;
	int jNext = (int)nextPosition.x;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		nextPosition += playerPositionDelta * player_speed * dt;
		iNext = (int)(nextPosition.y + (playerPositionDelta.y * collisionDistance));
		jNext = (int)(nextPosition.x + (playerPositionDelta.x * collisionDistance));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		nextPosition -= playerPositionDelta * player_speed * dt;
		iNext = (int)(nextPosition.y - (playerPositionDelta.y * collisionDistance));
		jNext = (int)(nextPosition.x - (playerPositionDelta.x * collisionDistance));
	}
	if (mapWalls[iNow * map_width + jNext] == 0)
		playerPosition.x = nextPosition.x;
	if (mapWalls[iNext * map_width + jNow] == 0)
		playerPosition.y = nextPosition.y;
}
	
void drawMap2D() {
	for (int i = 0; i < map_height; i++) {
		for (int j = 0; j < map_width; j++) {
			glm::vec3 color;
			if (mapWalls[i * map_width + j] > 0) color = glm::vec3(1, 1, 1);
			else color = glm::vec3(0, 0, 0);
			drawer.fillRect(
				j * tile_size + 1, i * tile_size + 1,
				tile_size - 2, tile_size - 2,
				color, drawerProgram);
		}
	}
}

void drawPlayer2D() {
	drawer.drawPoint(playerPosition.x * tile_size, playerPosition.y * tile_size, glm::vec3(1, 1, 0), 10, drawerProgram);
	drawer.drawLine(playerPosition.x * tile_size, playerPosition.y * tile_size,
		(playerPosition.x + playerPositionDelta.x * 0.5f) * tile_size,
		(playerPosition.y + playerPositionDelta.y * 0.5f) * tile_size,
		glm::vec3(1, 1, 0), 3, drawerProgram);
}

void drawRays2D3D() {
	float rayAngle = player_angle + DEG * fov / 2 * rayScale;
	rayAngle = limitAngle(rayAngle);

	for (int i = 0; i < fov * rayScale; i++) {
		int verticalWallType = 0;
		int horizontalWallType = 0;
		glm::vec2 rayHit{};
		glm::vec2 rayOffset{};
		//glm::vec2 playerPosModel = playerPosition;// / tile_size;
		float maxDepth = 8;
		float currentDepth = 0;
		
		// vertical lines checking
		if (rayAngle < PI/2 || rayAngle > 3*PI/2) { // looking right
			rayHit.x = (int)playerPosition.x + 1;
			float w = rayHit.x - playerPosition.x;
			rayHit.y = playerPosition.y - tanf(rayAngle) * w;
			//drawer.drawPoint(rayHit.x * tile_size, rayHit.y * tile_size, shaderProgram);
			rayOffset.y = -tanf(rayAngle);
			rayOffset.x = 1;
			//drawer.drawPoint((rayHit.x + rayOffset.x) * tile_size, (rayHit.y + rayOffset.y) * tile_size,
			//	glm::vec3(0, 0, 1), 10, shaderProgram);
		}
		if (rayAngle > PI/2 && rayAngle < 3*PI/2) { // looking left
			rayHit.x = (int)playerPosition.x;
			float w = playerPosition.x - rayHit.x;
			rayHit.y = playerPosition.y + tanf(rayAngle) * w;
			//drawer.drawPoint(rayHit.x * tile_size, rayHit.y * tile_size, shaderProgram);
			rayOffset.y = tanf(rayAngle);
			rayOffset.x = -1;
			//drawer.drawPoint((rayHit.x + rayOffset.x) * tile_size, (rayHit.y + rayOffset.y) * tile_size,
			//	glm::vec3(0, 0, 1), 10, shaderProgram);
		}
		if (rayAngle == PI/2 || rayAngle == 3*PI/2) {
			currentDepth = maxDepth;
			rayHit = playerPosition;
		}

		//printf("x=%3.3f\ty=%3.3f\n", rayHit.x, rayHit.y);
		//printf("i=%d\tj=%d\n", (int)rayHit.y - 1, (int)rayHit.x);

		while (currentDepth < maxDepth) {
			//drawer.drawPoint(rayHit.x * tile_size, rayHit.y * tile_size, glm::vec3(1, 0, 0), 10, shaderProgram);

			int i = (int)rayHit.y;
			int j = (int)rayHit.x - 1;
			if (rayAngle < PI / 2 || rayAngle > 3 * PI / 2) j += 1;
			int idx = i * map_width + j;
			if (i >= 0 && i < map_height &&
				j >= 0 && j < map_width &&
				mapWalls[idx] > 0) { // hit a wall
				currentDepth = maxDepth;
				verticalWallType = mapWalls[idx];
			}
			else {
				rayHit += rayOffset;
				currentDepth += 1;
			}
		}

		glm::vec2 verticalHit = rayHit;
		//drawer.drawLine(playerPosition.x, playerPosition.y, rayHit.x * tile_size, rayHit.y * tile_size, glm::vec3(1, 0, 0), 10, shaderProgram);

		// horizontal lines checking
		currentDepth = 0;
		if (rayAngle < PI) { // looking up
			rayHit.y = (int)playerPosition.y;
			float h = (playerPosition.y - rayHit.y);
			rayHit.x = playerPosition.x - (tanf(rayAngle - (PI / 2)) * h);
			//drawer.drawPoint(rayHit.x * tile_size, rayHit.y * tile_size, shaderProgram);
			rayOffset.y = -1;
			rayOffset.x = -tanf(rayAngle - (PI / 2));
			//drawer.drawPoint((rayHit.x + rayOffset.x) * tile_size, (rayHit.y + rayOffset.y) * tile_size,
			//	glm::vec3(0, 0, 1), 10, shaderProgram);
		}
		if (rayAngle > PI) { // looking down
			rayHit.y = (int)playerPosition.y + 1;
			float h = (rayHit.y - playerPosition.y);
			rayHit.x = playerPosition.x + (tanf(rayAngle - (PI / 2)) * h);
			//drawer.drawPoint(rayHit.x * tile_size, rayHit.y * tile_size, shaderProgram);
			rayOffset.y = 1;
			rayOffset.x = tanf(rayAngle - (PI / 2));
			//drawer.drawPoint((rayHit.x + rayOffset.x) * tile_size, (rayHit.y + rayOffset.y) * tile_size,
			//	glm::vec3(0, 0, 1), 10, shaderProgram);
		}
		if (rayAngle == 0.f || rayAngle == PI) {
			currentDepth = maxDepth;
			rayHit = playerPosition;
		}

		//printf("x=%3.3f\ty=%3.3f\n", rayHit.x, rayHit.y);
		//printf("i=%d\tj=%d\n", (int)rayHit.y - 1, (int)rayHit.x);

		while (currentDepth < maxDepth) {
			//drawer.drawPoint(rayHit.x * tile_size, rayHit.y * tile_size, glm::vec3(1, 0, 0), 10, shaderProgram);

			int i = (int)rayHit.y -1;
			if (rayAngle > PI) i += 1;
			int j = (int)rayHit.x;
			int idx = i * map_width + j;
			if (i >= 0 && i < map_height &&
				j >= 0 && j < map_width &&
				mapWalls[idx] > 0) { // hit a wall
				currentDepth = maxDepth;
				horizontalWallType = mapWalls[idx];
			}
			else {
				rayHit += rayOffset;
				currentDepth += 1;
			}
		}

		glm::vec2 horizontalHit = rayHit;
		//drawer.drawLine(playerPosition.x, playerPosition.y, rayHit.x * tile_size, rayHit.y * tile_size, glm::vec3(0, 1, 0), 5, shaderProgram);

		float distanceVertical = glm::distance(playerPosition, verticalHit);
		float distanceHorizontal = glm::distance(playerPosition, horizontalHit);

		float finalDistance = 1000000000;
		glm::vec3 wallColor = glm::vec3(0.0, 0.0, 0.0);
		bool verticalWall = true;
		int wallType = 0;

		if(distanceVertical < distanceHorizontal) {
			//drawer.drawLine(playerPosition.x * tile_size, playerPosition.y * tile_size,
			//	verticalHit.x * tile_size, verticalHit.y * tile_size, glm::vec3(1, 0, 0), 3, drawerProgram);
			finalDistance = distanceVertical;
		}
		else if(distanceVertical > distanceHorizontal){
			//drawer.drawLine(playerPosition.x * tile_size, playerPosition.y* tile_size,
			//	horizontalHit.x* tile_size, horizontalHit.y* tile_size, glm::vec3(1, 0, 0), 3, drawerProgram);
			finalDistance = distanceHorizontal;
			verticalWall = false;
		}
		if (verticalWall)
			wallType = verticalWallType;
		else
			wallType = horizontalWallType;

		float correctionAngle = limitAngle(player_angle - rayAngle);
		finalDistance *= cosf(correctionAngle);

		// Color
		if (wallType == 1)
			wallColor = glm::vec3(1, 0, 0);
		else if (wallType == 2)
			wallColor = glm::vec3(0, 0, 1);
		// Shade
		if (verticalWall)
			wallColor *= 0.8;
		else
			wallColor *= 0.5;
		// Fading
		wallColor *= (1 - finalDistance / maxDepth);

		

		//float maxLineHeight = 3*viewport_height/4;
		float maxLineHeight = bufferHeight;
		float lineHeight = maxLineHeight / finalDistance;
		if (lineHeight > maxLineHeight) lineHeight = maxLineHeight;

		//int idk = 10 / rayScale;
		//int idk = 10 / rayScale;
		//float lineOffset = (maxLineHeight - lineHeight) / 2 + viewport_height / 9;
		float lineOffset = (maxLineHeight - lineHeight) / 2;
		glm::vec2 lineStart = glm::vec2(i, lineOffset);
		glm::vec2 lineEnd = glm::vec2(i, lineHeight + lineOffset);

		// Draw the wall
		for (int y = 0; y < lineHeight; y++) {
			//drawer.drawPoint(lineStart.x, lineStart.y + y, wallColor, idk, drawerProgram);
			screenBuffer.setPixel(glm::vec4(wallColor, 1.f), lineStart.x, lineStart.y + y);
		}

		//drawer.drawLine(lineStart.x, lineStart.y, lineEnd.x, lineEnd.y, wallColor, idk, shaderProgram);

		rayAngle -= DEG;
		rayAngle = limitAngle(rayAngle);
	}
}

float limitAngle(float rad) {
	rad = fmod(rad, 2 * PI);
	if (rad < 0) rad += 2 * PI;
	return rad;
}