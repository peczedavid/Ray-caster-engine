#include "PrimitiveDrawer.h"

PrimitiveDrawer::PrimitiveDrawer() {
	this->initLineBuffer();
	this->lineWidth = 5.f;
	this->lineColor = glm::vec3(1, 0, 1);

	this->initPointBuffer();
	this->pointSize = 7.5f;
	this->pointColor = glm::vec3(1, 0, 1);
}

void PrimitiveDrawer::setSize(int width, int height) {
	this->width = width;
	this->height = height;
}

#pragma region POINT
void PrimitiveDrawer::drawPoint(float px, float py, const glm::vec3& color, float pointSize, ShaderProgram& shaderProgram) {
	glBindBuffer(GL_ARRAY_BUFFER, this->vboPoint);

	px = (px / this->width) * 2.f - 1.f;
	py = (1.f - (py/ this->height)) * 2 - 1.f;
	float positions[2] = { px, py };

	// Set the endpoints
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2, &positions, GL_STATIC_DRAW);

	// Set the color
	shaderProgram.setUniform(color, std::string("color"));

	// Set the width
	glPointSize(pointSize);

	glBindVertexArray(this->vaoPoint);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

void PrimitiveDrawer::drawPoint(float px, float py, ShaderProgram& shaderProgram) {
	this->drawPoint(px, py, this->pointColor, this->pointSize, shaderProgram);
}

void PrimitiveDrawer::initPointBuffer() {
	glGenBuffers(1, &this->vboPoint);
	glGenVertexArrays(1, &this->vaoPoint);

	glBindVertexArray(this->vaoPoint);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboPoint);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glBindVertexArray(0);
}
#pragma endregion

#pragma region LINE
void PrimitiveDrawer::drawLine(float start_x, float start_y, float end_x, float end_y, const glm::vec3& color, float lineWidth, ShaderProgram& shaderProgram) {
	glBindBuffer(GL_ARRAY_BUFFER, this->vboLine);

	start_x = (start_x / this->width) * 2.f - 1.f;
	start_y = (1.f - (start_y / this->height)) * 2 - 1.f;
	end_x = (end_x / this->width) * 2.f - 1.f;
	end_y = (1.f - (end_y / this->height)) * 2.f - 1.f;
	float positions[4] = { start_x, start_y, end_x, end_y };

	// Set the endpoints
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4, &positions, GL_STATIC_DRAW);

	// Set the color
	shaderProgram.setUniform(color, std::string("color"));

	// Set the width
	glLineWidth(lineWidth);

	glBindVertexArray(this->vaoLine);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void PrimitiveDrawer::drawLine(float start_x, float start_y, float end_x, float end_y, ShaderProgram& shaderProgram) {
	this->drawLine(start_x, start_y, end_x, end_y, this->lineColor, this->lineWidth, shaderProgram);
}

void PrimitiveDrawer::initLineBuffer() {
	glGenBuffers(1, &this->vboLine);
	glGenVertexArrays(1, &this->vaoLine);

	glBindVertexArray(this->vaoLine);
	glBindBuffer(GL_ARRAY_BUFFER, this->vboLine);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
	glBindVertexArray(0);
}
#pragma endregion

