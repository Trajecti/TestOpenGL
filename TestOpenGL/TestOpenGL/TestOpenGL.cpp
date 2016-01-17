// TestOpenGL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>


//screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


//initialize program
bool init();

GLuint positionBufferObject;
GLuint theProgram;

const float vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
};

void InitializeVertexBuffer() {

	//creates buffer object to positionBuffer Object handle
	glGenBuffers(1, &positionBufferObject);

	//binds buffer object to binding target
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);

	//allocates  to OpenGl size of memory of data and copy data to it
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

	//unbinds buffer using 0 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile) {
	// determines shader type and reads from shader file
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	//compiles shader
	glCompileShader(shader);

	//checks if compilation was succesful
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	//if not, return pertinent error message
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch (eShaderType){
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint CreateProgram(const std::vector<GLuint> &shaderList) {
	GLuint program = glCreateProgram();

	for (size_t i = 0; i < shaderList.size(); i++) {
		glAttachShader(program, shaderList[i]);
	}

	//attach shaders and links them into a program
	glLinkProgram(program);

	//checks if linking was succesful
	GLint status;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &status);

	//if not, return error message
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	//remove unneeded shaders
	for (size_t i = 0; i < shaderList.size(); i++) {
		glDetachShader(program, shaderList[i]);
	}

	return program;
}
void InitializeProgram() {
	//creates a list of shader objects we are looking for
	std::vector<GLuint> shaderList;
	std::string strVertexShader = "vertex_shader.txt";
	std::string strFragmentShader = "fragment_shader.txt";

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

	theProgram = CreateProgram(shaderList);

	//deletes shaders after use
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}


void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void display() {

	//clears screen buffer using the colour black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//uses the program created
	glUseProgram(theProgram);


	//binds array buffer the buffer object created and passes info on attribute array index 0
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	//draws triangles
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
	glUseProgram(0);

	glutSwapBuffers();
}

int main(int argc, char** argv) {

	// initializing freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(250, 250);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Test");

	//intializing glewInit
	GLenum err = glewInit();

	//initalizing programs
	InitializeProgram();
	InitializeVertexBuffer();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutMainLoop();
	
	return 0;
}

