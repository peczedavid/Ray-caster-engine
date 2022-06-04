#ifndef PRIMITIVE_DRAWER_H
#define PRIMITIVE_DRAWER_H

#include <glm/vec3.hpp>
#include "ShaderProgram.h"

class PrimitiveDrawer {
public:
	PrimitiveDrawer();
	void init();
	void setSize(int width, int height);

private:
	int width, height;

#pragma region RECTANGLE
public:
	void fillRect(float px, float py, float rect_width, float rect_height, const glm::vec3& color, ShaderProgram& shaderProgram);
	void fillRect(float px, float py, float rect_width, float rect_height, ShaderProgram& shaderProgram);

	glm::vec3 rectColor;

private:
	unsigned int vboRect, vaoRect, eboRect;
	void initRectBuffer();
#pragma endregion

#pragma region POINT
public:
	void drawPoint(float px, float py, const glm::vec3& color, float pointSize, ShaderProgram& shaderProgram);
	void drawPoint(float px, float py,ShaderProgram& shaderProgram);

	glm::vec3 pointColor;
	float pointSize;

private:
	unsigned int vboPoint, vaoPoint;
	void initPointBuffer();
#pragma endregion

#pragma region LINE
public:
	void drawLine(float start_x, float start_y, float end_x, float end_y, const glm::vec3& color, float lineWidth, ShaderProgram& shaderProgram);
	void drawLine(float start_x, float start_y, float end_x, float end_y, ShaderProgram& shaderProgram);

	glm::vec3 lineColor;
	float lineWidth;

private:
	unsigned int vboLine, vaoLine;
	void initLineBuffer();
#pragma endregion
};

#endif