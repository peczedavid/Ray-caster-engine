#ifndef TEXTURE_H
#define TEXTURE_H

#include <glm/vec4.hpp>
#include <stb/stb_image.h>
#include <vector>
#include <string>

class Texture {
public:
	Texture();

	Texture(int width, int height, std::vector<glm::vec4> pixelBuffer);

	Texture(int width, int height, const char* filename);

	glm::vec4 sample(int x, int y);
	std::vector<glm::vec4> m_pixelBuffer;
private:
	int m_Width, m_Height;
};

#endif