#include "Texture.h"

Texture::Texture() :
	m_Width(0), m_Height(0) { }

Texture::Texture(int width, int height, std::vector<glm::vec4> pixelBuffer) : 
	m_Width(width), m_Height(height), m_pixelBuffer(pixelBuffer) { }

Texture::Texture(int width, int height, const char* filename) {
	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* bytes = stbi_load(filename, &widthImg, &heightImg, &numColCh, 4);
	if (widthImg != width || heightImg != height || !bytes) {
		printf("Wrong size:\n");
		printf("Expected: %dx%d - Actual: %dx%d", width, height, widthImg, heightImg);
		m_pixelBuffer.resize(width * height);
		stbi_image_free(bytes);
		return;
	}

	m_Width = widthImg;
	m_Height = heightImg;

	m_pixelBuffer.resize(m_Width * m_Height);

	unsigned bytePerPixel = numColCh;

	for (int y = 0; y < m_Height; y++) {
		for (int x = 0; x < m_Width; x++) {
			int i = y * m_Width + x;
			unsigned char* pixelOffset = bytes + (m_Width * y + x) * bytePerPixel;
			unsigned char r = pixelOffset[0];
			unsigned char g = pixelOffset[1];
			unsigned char b = pixelOffset[2];
			m_pixelBuffer[i] = glm::vec4(r/255.f, g/255.f, b/255.f, 1.f);
		}
	}

	stbi_image_free(bytes);
}

glm::vec4 Texture::sample(int x, int y) {
	if (x < 0 || x >= m_Width) return glm::vec4(0.f, 0.f, 0.f, 1.f);
	if (y < 0 || y >= m_Height) return glm::vec4(0.f, 0.f, 0.f, 1.f);

	return m_pixelBuffer[y * m_Width + x];
}
