// TestOpenGL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestOpenGL.h"



//screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


// ids for program and vertex positions
GLuint vertexBufferObject;
GLuint theProgram;
GLuint vao;

const float vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
};

void InitializeVertexBuffer() {

	//creates buffer object to positionBuffer Object handle
	glGenBuffers(1, &vertexBufferObject);

	//binds buffer object to binding target
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	//allocates  to OpenGl size of memory of data and copy data to it
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

	//unbinds buffer using 0 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeVertexBuffer();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}



void InitializeProgram() {
	//creates a list of shader objects we are looking for
	std::vector<GLuint> shaderList;
	std::string strVertexShader = "vertex_shader.txt";
	std::string strFragmentShader = "fragment_shader.txt";

	std::string test = Framework::LoadFile(strFragmentShader);
	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, Framework::LoadFile(strVertexShader)));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, Framework::LoadFile(strFragmentShader)));

	theProgram = Framework::CreateProgram(shaderList);

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
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	//draws triangles
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
	glUseProgram(0);

	glutSwapBuffers();
}


