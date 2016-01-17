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
GLuint vao, offsetLocation;

const float vertexPositions[] = {
	0.25f, 0.25f, 0.0f, 1.0f,
	0.25f, -0.25f, 0.0f, 1.0f,
	-0.25f, -0.25f, 0.0f, 1.0f,
};

void ComputePositionOffsets(float &fXOffset, float &fYOffset) {
	//set loop duration to 5s
	const float fLoopDuration = 5.0f;
	const float fScale = 3.14159f * 2.0f;

	float fElapsedTime = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	float fCurrTimeThroughLoop = fmodf(fElapsedTime, fLoopDuration);

	// multiply by 1/2 to reduce circle to diameter of 1
	fXOffset = cosf(fCurrTimeThroughLoop * fScale) * 0.5f;
	fYOffset = sinf(fCurrTimeThroughLoop *fScale) * 0.5f;
}

void AdjustVertexData(float fXOffset, float fYOffset) {
	std::vector<float> fNewData(ARRAY_COUNT(vertexPositions));
	memcpy(&fNewData[0], vertexPositions, sizeof(vertexPositions));

	//changes positions by offset
	for (int i = 0; i < ARRAY_COUNT(vertexPositions); i += 4) {
		fNewData[i] += fXOffset;
		fNewData[i] += fYOffset;
	}

	//updates buffer with new data
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexPositions), &fNewData[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}



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
	std::string strVertexShader = "data/offset.vert";
	std::string strFragmentShader = "data/pos.frag";

	std::string test = Framework::LoadFile(strFragmentShader);
	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, Framework::LoadFile(strVertexShader)));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, Framework::LoadFile(strFragmentShader)));

	theProgram = Framework::CreateProgram(shaderList);

	offsetLocation = glGetUniformLocation(theProgram, "offset");
	//deletes shaders after use
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}


void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void display() {

	//generates offsets
	float fXOffset = 0.0f, fYOffset = 0.0f;
	ComputePositionOffsets(fXOffset, fYOffset);
	AdjustVertexData(fXOffset, fYOffset);

	//clears screen buffer using the colour black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//uses the program created
	glUseProgram(theProgram);

	glUniform2f(offsetLocation, fXOffset, fYOffset);

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


