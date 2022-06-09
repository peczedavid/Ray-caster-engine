#ifndef TEXTURE_H
#define TEXTURE_H

#include <glm/vec4.hpp>
#include <vector>

class Texture {
public:
	Texture();

	Texture(int width, int height, std::vector<glm::vec4>& pixelBuffer);

	glm::vec4 sample(int x, int y);
private:
	int m_Width, m_Height;
	std::vector<glm::vec4> m_pixelBuffer;
};

#endif