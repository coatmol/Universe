#include "File.h"

std::string get_file_contents(const char* filePath)
{
	std::ifstream file(filePath);
	if (!file)
	{
		std::cerr << "Error opening file: " << filePath << std::endl;
		return "";
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}