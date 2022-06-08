#include "ScreenBuffer.h"

ScreenBuffer::ScreenBuffer() { }

ScreenBuffer::ScreenBuffer(int bufferWidth, int bufferHeight, glm::vec4 backgroundColor) {
	m_bufferWidth = bufferWidth;
	m_bufferHeight = bufferHeight;
	m_backgroundColor = backgroundColor;

	// NDC
	std::vector<glm::vec2> bufferVertices = {
		glm::vec2(-1.f, -1.f), glm::vec2(1.f, -1.f), glm::vec2(1.f, 1.f), glm::vec2(-1.f, 1.f)
	};

	m_pixelBuffer.resize(m_bufferWidth * m_bufferHeight);

	m_screenShader = ShaderProgram("screen.vert", "screen.frag", "bufferPixelColor");
	initTexture();

	initBuffers(bufferVertices);
}

ScreenBuffer::ScreenBuffer(int bufferWidth, int bufferHeight, glm::vec4 backgroundColor, std::vector<glm::vec2>& vertices) {
	m_bufferWidth = bufferWidth;
	m_bufferHeight = bufferHeight;
	m_backgroundColor = backgroundColor;

	m_pixelBuffer.resize(m_bufferWidth * m_bufferHeight);

	m_screenShader = ShaderProgram("screen.vert", "screen.frag", "bufferPixelColor");
	initTexture();

	initBuffers(vertices);
}

void ScreenBuffer::initBuffers(std::vector<glm::vec2>& vertices) {
	// Init buffers
	glGenBuffers(1, &m_verticesVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_verticesVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_texCoordVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
	glm::vec2 texCoords[4] = {
		glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1)
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, &texCoords[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_indicesEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesEbo);
	unsigned int indices[6] = {
		0, 1, 3,
		1, 2, 3
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, &indices[0], GL_STATIC_DRAW);

	// Init layout
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_verticesVbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVbo);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ScreenBuffer::setPixel(glm::vec3 color, int x, int y) {
	if (x < 0 || x >= m_bufferWidth) return;
	if (y < 0 || y >= m_bufferHeight) return;

	int idx = ((m_bufferHeight - 1) - y) * m_bufferWidth + x;
	m_pixelBuffer[idx].x = color.x;
	m_pixelBuffer[idx].y = color.y;
	m_pixelBuffer[idx].z = color.z;
}

void ScreenBuffer::setPixel(glm::vec4 color, int x, int y) {
	if (x < 0 || x >= m_bufferWidth) return;
	if (y < 0 || y >= m_bufferHeight) return;

	int idx = ((m_bufferHeight - 1) - y) * m_bufferWidth + x;
	m_pixelBuffer[idx] = color;
}

void ScreenBuffer::drawBuffer() {
	m_screenShader.Use();
	bindTexture();
	m_screenShader.setUniform(0, std::string("u_screenTexture"));

	// Upload the buffer to the GPU
	glBindTexture(GL_TEXTURE_2D, m_pixelBufferId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_bufferWidth, m_bufferHeight, 0, GL_RGBA, GL_FLOAT, &m_pixelBuffer[0]);

	// Draw the buffer
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesEbo);
	

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

	// Unbind 
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	unbindTexture();

	clearBuffer();
}

void ScreenBuffer::resize(int bufferWidth, int bufferHeight) {
	m_bufferWidth = bufferWidth;
	m_bufferHeight = bufferHeight;

	m_pixelBuffer.resize(m_bufferWidth * m_bufferHeight);
	clearBuffer();
}

void ScreenBuffer::clearBuffer() {
	for (auto& pixel : m_pixelBuffer)
		pixel = m_backgroundColor;
}

void ScreenBuffer::initTexture() {
	glGenTextures(1, &m_pixelBufferId);

	glBindTexture(GL_TEXTURE_2D, m_pixelBufferId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	unbindTexture();
}

void ScreenBuffer::bindTexture() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pixelBufferId);
}

void ScreenBuffer::unbindTexture() {
	glBindTexture(GL_TEXTURE_2D, 0);
}


