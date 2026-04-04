#include "Shader.h"



Shader::Shader(const char* vertexPath, const char* fragmentPath) {}


std::string Shader::readFile(std::string path) {
	std::ifstream readFile(path);

	if (!readFile.is_open()) {
		std::cerr << "Error: File was not opened!" << std::endl;
		return std::string();
	}

	std::stringstream ss;

	ss << readFile.rdbuf();

	return ss.str();
}