#pragma once
#include <glad/glad.h>
#include "../src/utils/File.h"

class Shader
{
public:
	Shader(const char* vertexFile, const char* fragmentFile);

	void Activate();
	void Delete();
private:
	GLuint Program;
};