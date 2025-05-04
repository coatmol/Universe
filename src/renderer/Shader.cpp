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
	compileError(vertexShader, "VERTEX");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);
	compileError(vertexShader, "FRAGMENT");

	ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vertexShader);
	glAttachShader(ProgramID, fragmentShader);
	glLinkProgram(ProgramID);
	compileError(vertexShader, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(vertexShader);
}

void Shader::Activate()
{
	glUseProgram(ProgramID);
}

void Shader::Delete()
{
	glDeleteProgram(ProgramID);
}

void Shader::compileError(GLuint shader, const char* type)
{
	GLint success;
	char infoLog[512];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER_COMPILATION_ERROR: " << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER_LINKING_ERROR: " << type << "\n" << infoLog << std::endl;
		}
	}
}
