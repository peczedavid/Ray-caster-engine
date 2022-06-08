#ifndef SCREEN_BUFFER_H
#define SCREEN_BUFFER_H

#include "ShaderProgram.h"
#include <vector>

// 3-------2
// | \     |
// |   \   |
// |     \ |
// O-------1

class ScreenBuffer {
public:
	ScreenBuffer();

	ScreenBuffer(int bufferWidth, int bufferHeight, glm::vec4 backgroundColor); // fullscreen

	ScreenBuffer(int bufferWidth, int bufferHeight, glm::vec4 backgroundColor, std::vector<glm::vec2>& vertices); // given positin in NDC

	void setPixel(glm::vec3 color, int x, int y);

	void setPixel(glm::vec4 color, int x, int y);

	void drawBuffer();

	void resize(int bufferWidth, int bufferHeight);
private:
	void initBuffers(std::vector<glm::vec2>& vertices);

	void clearBuffer(); // fills pixels with background color

	void initTexture(); // initializes 

	void bindTexture(); // always slot 0 in m_screenShader

	void unbindTexture();

	int m_bufferWidth, m_bufferHeight;
	ShaderProgram m_screenShader; // texture drawer

	unsigned int m_pixelBufferId;
	std::vector<glm::vec4> m_pixelBuffer; // texture data
	glm::vec4 m_backgroundColor; // base color

	unsigned int m_vao;
	unsigned int m_verticesVbo, m_texCoordVbo;
	unsigned int m_indicesEbo;
};

#endif