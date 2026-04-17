#include "gfx/Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	std::string vertexCode = readFile(vertexPath);
	std::string fragmentCode = readFile(fragmentPath);

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};


	// shader Program (linking)
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


int Shader::getUniformLocation(const std::string& name) {
	auto it = Location.find(name);
	if (it != Location.end()) {
		return it->second; // return cached location
	}

	int location = glGetUniformLocation(this->ID, name.c_str());

	if (location == -1) {
		std::cerr << "Warning: Uniform '" << name << "' not found in shader!" << std::endl;
	}

	Location[name] = location;
	return location;
}

void Shader::use() {
	glUseProgram(ID);
}

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

Shader::~Shader() {
	glDeleteProgram(ID);
}