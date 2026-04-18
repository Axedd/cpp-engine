#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

class Shader {
public:
	// program ID
	unsigned int ID;

	// constructor
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	// use/activate the shader
	void use();
	void setBool(int location, bool value) const;
	void setInt(int location, int value) const;
	void setFloat(int location, float value) const;


private:
	std::string readFile(std::string pat);
	std::string vertexPath;
	std::string fragmentPath;

	int getUniformLocation(const std::string& name);

	std::unordered_map<std::string, int> Location;
};


#endif // !SHADER_H



