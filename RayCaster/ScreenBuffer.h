#ifndef SCREEN_BUFFER_H
#define SCREEN_BUFFER_H

#include "ShaderProgram.h"
#include <vector>

class ScreenBuffer {
public:
	ScreenBuffer();

	ScreenBuffer(int bufferWidth, int bufferHeight); // fullscreen

	ScreenBuffer(int bufferWidth, int bufferHeight, std::vector<glm::vec2> vertices); // given positin in NDC

	void setPixel(glm::vec3 color, int x, int y);

	void drawBuffer();
private:
	int bufferWidth, bufferHeight;
	ShaderProgram screenShader; // texture drawer

	std::vector<glm::vec4> pixelBuffer; // texture data

	int bufferVao, bufferVbo;
	std::vector<glm::vec2> vertices; // NDC
};

#endif