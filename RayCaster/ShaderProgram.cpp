#include "ShaderProgram.h"

std::string getFileAsString(const char* fileName)
{
	std::ifstream in(fileName, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

GLuint ShaderProgram::getUniformLocation(const std::string& name)
{
	GLuint location = glGetUniformLocation(shaderProgramId, name.c_str());
	if (location < 0) printf("uniform %s cannot be set\n", name.c_str());
	return location;
}

ShaderProgram::ShaderProgram(const char* vertexFileName, const char* fragmentFileName, const char* fragmentOutputName)
{
	std::string vertexCode = getFileAsString(vertexFileName);
	std::string fragmentCode = getFileAsString(fragmentFileName);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexSource, NULL);
	glCompileShader(vertexShaderId);

	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShaderId);

	shaderProgramId = glCreateProgram();
	glAttachShader(shaderProgramId, vertexShaderId);
	glAttachShader(shaderProgramId, fragmentShaderId);

	glLinkProgram(shaderProgramId);
	glBindFragDataLocation(shaderProgramId, 0, fragmentOutputName);

	Use();
}

ShaderProgram::~ShaderProgram()
{
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
	Delete();
}

GLuint ShaderProgram::getId()
{
	return this->shaderProgramId;
}

void ShaderProgram::Use()
{
	glUseProgram(shaderProgramId);
}

void ShaderProgram::Delete()
{
	if (shaderProgramId > 0)
		glDeleteProgram(shaderProgramId);
}

void ShaderProgram::setUniform(GLuint i, const std::string& name)
{
	GLuint location = getUniformLocation(name);
	if (location >= 0) glUniform1i(location, i);
}

void ShaderProgram::setUniform(GLfloat f, const std::string& name)
{
	GLuint location = getUniformLocation(name);
	if (location >= 0) glUniform1f(location, f);
}

void ShaderProgram::setUniform(const glm::vec3& vec, const std::string& name)
{
	GLuint location = getUniformLocation(name);
	if (location >= 0) glUniform3f(location, vec.x, vec.y, vec.z);
}

void ShaderProgram::setUniform(const glm::vec4& vec, const std::string& name)
{
	GLuint location = getUniformLocation(name);
	if (location >= 0) glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}

void ShaderProgram::setUniform(const glm::mat4& mat, const std::string& name)
{
	GLuint location = getUniformLocation(name);
	if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}
