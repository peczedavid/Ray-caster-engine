#include "Texture.h"

Texture::Texture() :
	m_Width(0), m_Height(0) { }

Texture::Texture(int width, int height, std::vector<glm::vec4>& pixelBuffer) : 
	m_Width(width), m_Height(height), m_pixelBuffer(pixelBuffer) { }

glm::vec4 Texture::sample(int x, int y) {
	if (x < 0 || x >= m_Width) return glm::vec4(0.f, 0.f, 0.f, 0.f);
	if (y < 0 || y >= m_Height) return glm::vec4(0.f, 0.f, 0.f, 0.f);

	return m_pixelBuffer[y * m_Width + x];
}
