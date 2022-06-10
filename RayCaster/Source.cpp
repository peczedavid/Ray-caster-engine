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

float rayScale = 10.0f;
constexpr float fov = 60; // degrees
constexpr float PI = 3.14159265359f;
float DEG = 0.0174532925f / rayScale;

int bufferWidth = fov * rayScale, bufferHeight = 0.75f * bufferWidth;
ScreenBuffer screenBuffer;
ShaderProgram drawerProgram;
PrimitiveDrawer drawer;

std::vector<Texture> textures;

GLFWwindow* window;
int viewport_width = 1000;
int viewport_height = 0.75f * viewport_width;
int texture_width = 64;
int texture_height = 64;

float t = 0, dt = 0;
float player_speed = 1.5f;
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
float degToRad(float deg);

std::vector<glm::vec4> genCheckerBoardTexture(int xDim, int yDim);

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

	textures.resize(3);
	textures[0] = Texture(texture_width, texture_height, "blue-wall.png");
	textures[1] = Texture(texture_width, texture_height, "red-wall.png");
	//textures[2] = Texture(texture_width, texture_height, "floor.png");
	textures[2] = Texture(texture_width, texture_height, "xd1.png");

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
	rayScale = glm::clamp(rayScale, 0.1f, 10.f);

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

	float sprintMultiplier = 1.f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		sprintMultiplier = 2.f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		nextPosition += playerPositionDelta * player_speed * sprintMultiplier * dt;
		iNext = (int)(nextPosition.y + (playerPositionDelta.y * collisionDistance));
		jNext = (int)(nextPosition.x + (playerPositionDelta.x * collisionDistance));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		nextPosition -= playerPositionDelta * player_speed * sprintMultiplier * dt;
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
		float maxDepth = 10;
		float currentDepth = 0;
		
		// vertical lines checking
		if (rayAngle < PI/2 || rayAngle > 3*PI/2) { // right
			rayHit.x = (int)playerPosition.x + 1;
			float w = rayHit.x - playerPosition.x;
			rayHit.y = playerPosition.y - tanf(rayAngle) * w;
			rayOffset.y = -tanf(rayAngle);
			rayOffset.x = 1;
		}
		if (rayAngle > (PI/2.f) && rayAngle < (3.f*PI/2.f)) { // left
			rayHit.x = (int)playerPosition.x;
			float w = playerPosition.x - rayHit.x;
			rayHit.y = playerPosition.y + tanf(rayAngle) * w;
			rayOffset.y = tanf(rayAngle);
			rayOffset.x = -1;
		}
		if (rayAngle == PI/2 || rayAngle == 3*PI/2) {
			currentDepth = maxDepth;
			rayHit = playerPosition;
		}

		while (currentDepth < maxDepth) {
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

		// horizontal lines checking
		currentDepth = 0;
		if (rayAngle < PI) { //  up
			rayHit.y = (int)playerPosition.y;
			float h = (playerPosition.y - rayHit.y);
			rayHit.x = playerPosition.x - (tanf(rayAngle - (PI / 2)) * h);
			rayOffset.y = -1;
			rayOffset.x = -tanf(rayAngle - (PI / 2));
		}
		if (rayAngle > PI) { //  down
			rayHit.y = (int)playerPosition.y + 1;
			float h = (rayHit.y - playerPosition.y);
			rayHit.x = playerPosition.x + (tanf(rayAngle - (PI / 2)) * h);
			rayOffset.y = 1;
			rayOffset.x = tanf(rayAngle - (PI / 2));
		}
		if (rayAngle == 0.f || rayAngle == PI) {
			currentDepth = maxDepth;
			rayHit = playerPosition;
		}

		while (currentDepth < maxDepth) {
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

		float distanceVertical = glm::distance(playerPosition, verticalHit);
		float distanceHorizontal = glm::distance(playerPosition, horizontalHit);

		float finalDistance = 1000000000;
		glm::vec2 finalHit{};
		bool verticalWall = true;
		int wallType = 0;

		if(distanceVertical < distanceHorizontal) {
			finalDistance = distanceVertical;
			finalHit = verticalHit;
		}
		else if(distanceVertical > distanceHorizontal){
			finalDistance = distanceHorizontal;
			finalHit = horizontalHit;
			verticalWall = false;
		}
		if (verticalWall)
			wallType = verticalWallType;
		else
			wallType = horizontalWallType;

		float correctionAngle = limitAngle(player_angle - rayAngle);
		finalDistance *= cosf(correctionAngle);

		float maxLineHeight = bufferHeight;
		float lineHeight = maxLineHeight / finalDistance;
		float textureYStep = 1 / lineHeight;
		float textureYOffset = 0;
		if (lineHeight > maxLineHeight) {
			textureYOffset = (lineHeight - maxLineHeight) / 2.f;
			lineHeight = maxLineHeight;
		}

		float lineOffset = (maxLineHeight - lineHeight) / 2;
		glm::vec2 lineStart = glm::vec2(i, lineOffset);
		glm::vec2 lineEnd = glm::vec2(i, lineHeight + lineOffset);

		float textureY = textureYOffset * textureYStep;
		float textureX;
		if (verticalWall)
			textureX = (finalHit.y - (int)finalHit.y) * texture_width;
		else
			textureX = (finalHit.x - (int)finalHit.x) * texture_width;

		Texture texture;
		texture = textures[wallType - 1];
		glm::vec4 wallColor(0.0f, 0.0f, 0.0f, 1.f);

		// Draw the wall
		int y;
		for (y = 0; y < lineHeight; y++) {
			// Color
			int correctedY = textureY * texture_height;

			if (rayAngle > PI) {
				if (!verticalWall) {
					textureX = (1.f - (finalHit.x - (int)finalHit.x)) * texture_width;
				}
			}

			if (rayAngle > (PI / 2.f) && rayAngle < (3.f * PI / 2.f)) {
				if (verticalWall) {
					textureX = (1.f - (finalHit.y - (int)finalHit.y)) * texture_width;
				}
			}
			
			wallColor = texture.sample(textureX, correctedY);

			// Shade
			if (verticalWall)
				wallColor *= 0.6;

			// Fading
			wallColor *= (1 - (finalDistance / maxDepth) * 0.5f);
			screenBuffer.setPixel(wallColor, lineStart.x, lineStart.y + y);

			textureY += textureYStep;
		}

		// Draw the floor
		for(y = lineOffset + lineHeight; y < bufferHeight; y++) {
			float dy = y - (bufferHeight / 2.f);
			float deg = rayAngle;
			float dist = (bufferWidth / 2.f) / (tanf(degToRad(fov/2.f)));
			float raFix = cosf(limitAngle(rayAngle - player_angle));
			float tx = playerPosition.x/2.f + cosf(deg) * dist / dy / raFix;
			float ty = playerPosition.y/2.f - sinf(deg) * dist / dy / raFix;

			glm::vec4 floorColor = textures[2].sample(tx * texture_width, ty * texture_width);

			screenBuffer.setPixel(floorColor, lineStart.x, y);
		}

		rayAngle -= DEG;
		rayAngle = limitAngle(rayAngle);
	}
}

float degToRad(float deg) {
	return deg / (180.f / PI);
}

float limitAngle(float rad) {
	rad = fmod(rad, 2 * PI);
	if (rad < 0) rad += 2 * PI;
	return rad;
}

std::vector<glm::vec4> genCheckerBoardTexture(int xDim, int yDim) {
	std::vector<glm::vec4> checkerBoard;
	checkerBoard.resize(texture_width * texture_height);
	const glm::vec4 white(1.f, 1.f, 1.f, 1.f);
	const glm::vec4 black(0.f, 0.f, 0.f, 1.f);
	int tileWidth = texture_width / xDim;
	int tileHeight = texture_height / yDim;
	for (int y = 0; y < texture_height; y++) {
		for (int x = 0; x < texture_width; x++) {
			checkerBoard[x * texture_width + y] = (x & tileWidth) ^ (y & tileHeight) ? white : black;
		}
	}
	return checkerBoard;
}