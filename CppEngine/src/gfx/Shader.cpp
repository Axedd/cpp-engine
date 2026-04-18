#include "gfx/Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	std::string vertexCode = readFile(vertexPath);
	std::string fragmentCode = readFile(fragmentPath);

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	
	// GLSL-code text string sent to the GPU driver
	// Shader cannot be precompiled since there exists many forms of GPUs
	vertex = glCreateShader(GL_VERTEX_SHADER); // empty "container" with respective type here GL_VERTEX_SHADER = transform vertex from local space to Clip Space
	glShaderSource(vertex, 1, &vShaderCode, NULL); // Connect the GLSL code to the unique ID just created before
	glCompileShader(vertex); // Compile the Shader code to something the GPU understands
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success); // Used to validate glCompileShader

	// Compiler error
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	};

	// Same as the Vertex but for the Fragment (color of the pixels inside the vertex)
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	};


	// shader Program (linking Vertex and Fragment to validate if they match)
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);

	// If they don't match error occurs
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

// Caching 
// enables cummincation from C++ to the shader while it runs
// "Uniform" comes from the fact that the value is identical for all
// thousands of threads which runs in the draw-call
// so here we can send: time, colors and transform-matrices ...
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


// Helper for reading GLSL-code from files
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