// TestOpenGL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestOpenGL.h"

#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))


//screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


// ids for program and vertex positions
GLuint positionBufferObject;
GLuint theProgram;
GLuint vao, elapsedTimeUniform;

const float vertexPositions[] = {
	0.25f, 0.25f, 0.0f, 1.0f,
	0.25f, -0.25f, 0.0f, 1.0f,
	-0.25f, -0.25f, 0.0f, 1.0f,
};




void InitializeVertexBuffer() {

	//creates buffer object to positionBuffer Object handle
	glGenBuffers(1, &positionBufferObject);

	//binds buffer object to binding target
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);

	//allocates  to OpenGl size of memory of data and copy data to it
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STREAM_DRAW);

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
	std::string strVertexShader = "data/calcOffset.vert";
	std::string strFragmentShader = "data/calcColor.frag";

	std::string test = Framework::LoadFile(strFragmentShader);
	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, Framework::LoadFile(strVertexShader)));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, Framework::LoadFile(strFragmentShader)));

	theProgram = Framework::CreateProgram(shaderList);

	elapsedTimeUniform = glGetUniformLocation(theProgram, "time");

	GLuint loopDurationUnf = glGetUniformLocation(theProgram, "loopDuration");
	GLuint fragLoopDurUnf = glGetUniformLocation(theProgram, "fragLoopDuration");

	glUseProgram(theProgram);
	glUniform1f(loopDurationUnf, 5.0f);
	glUniform1f(fragLoopDurUnf, 10.0f);
	glUseProgram(0);
	//deletes shaders after use
	//std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
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

	glUniform1f(elapsedTimeUniform, glutGet(GLUT_ELAPSED_TIME) /1000.0f);

	//binds array buffer the buffer object created and passes info on attribute array index 0
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// the void (void*48) represents how much to offset in the array by 
	// 4 (the size of a float) * 4 (the number of floats in a vec4) 
	//* 3 (the number of vec4's in the position data).
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 48);
	
	//draws triangles
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	glUseProgram(0);

	glutSwapBuffers();
	glutPostRedisplay();
}


