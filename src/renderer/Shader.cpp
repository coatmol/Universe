#include "Shader.h"

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexShaderSource = get_file_contents(vertexFile);
	std::string fragmentShaderSource = get_file_contents(fragmentFile);

	const char* vertexSource = vertexShaderSource.c_str();
	const char* fragmentSource = fragmentShaderSource.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);

	Program = glCreateProgram();
	glAttachShader(Program, vertexShader);
	glAttachShader(Program, fragmentShader);
	glLinkProgram(Program);

	glDeleteShader(vertexShader);
	glDeleteShader(vertexShader);
}

void Shader::Activate()
{
	glUseProgram(Program);
}

void Shader::Delete()
{
	glDeleteProgram(Program);
}
