#pragma once

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

namespace Framework
{
	//load file into a strring
	std::string LoadFile(std::string path);

	//loads a shader from string
	GLuint LoadShader(GLenum eShaderType, std::string &strShaderFile);

	//creates program consisting of shaders
	GLuint CreateProgram(const std::vector<GLuint> &shaderList);
}