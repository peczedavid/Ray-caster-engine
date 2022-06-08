#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include<fstream>

std::string getFileAsString(const char* fileName);

class ShaderProgram
{
private:
	GLuint shaderProgramId;
	GLuint vertexShaderId, fragmentShaderId;

	GLuint getUniformLocation(const std::string& name);
public:
	ShaderProgram(const char* vertexFileName, const char* fragmentFileName, const char* fragmentOutputName = "outColor");
	ShaderProgram();
	~ShaderProgram();

	GLuint getId();
	void Use();
	void Delete();

	void setUniform(int i, const std::string& name);
	void setUniform(GLuint i, const std::string& name);
	void setUniform(GLfloat f, const std::string& name);
	void setUniform(const glm::vec3& vec, const std::string& name);
	void setUniform(const glm::vec4& vec, const std::string& name);
	void setUniform(const glm::mat4& mat, const std::string& name);
};

#endif
