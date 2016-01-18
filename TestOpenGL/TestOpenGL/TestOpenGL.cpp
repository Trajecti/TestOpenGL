// TestOpenGL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestOpenGL.h"
#include "mat4x4.hpp"
#include "vec3.hpp"
#include "gtc/type_ptr.hpp"
#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))


//screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;



//perspective matrix
glm::mat4 cameraToClipMatrix;


// ids for program and vertex positions
GLuint vertexBufferObject, indexBufferObject;
GLuint positionAttrib, colorAttrib;
GLuint theProgram, vao;
GLuint modelToCameraMatrixUnif, cameraToClipMatrixUnif, perspectiveMatrixUnif;



float CalcFrustumScale(float fFovDeg)
{
	const float degToRad = 3.14159f * 2.0f / 360.0f;
	float fFovRad = fFovDeg * degToRad;
	return 1.0f / tan(fFovRad / 2.0f);
}
const float fFrustumScale = CalcFrustumScale(45.0f);

void InitializeProgram() {
	//creates a list of shader objects we are looking for
	std::vector<GLuint> shaderList;
	std::string strVertexShader = "data/colorTransform.vert";
	std::string strFragmentShader = "data/color.frag";

	std::string test = Framework::LoadFile(strFragmentShader);
	shaderList.push_back(Framework::LoadShader(GL_VERTEX_SHADER, Framework::LoadFile(strVertexShader)));
	shaderList.push_back(Framework::LoadShader(GL_FRAGMENT_SHADER, Framework::LoadFile(strFragmentShader)));

	theProgram = Framework::CreateProgram(shaderList);
	positionAttrib = glGetAttribLocation(theProgram, "position");
	colorAttrib = glGetAttribLocation(theProgram, "color");

	modelToCameraMatrixUnif = glGetUniformLocation(theProgram, "modelToCameraMatrix");
	cameraToClipMatrixUnif = glGetUniformLocation(theProgram, "cameraToClipMatrix");

	float fzNear = 1.0f;
	float fzFar = 45.0f;

	cameraToClipMatrix[0].x = fFrustumScale;
	cameraToClipMatrix[1].y = fFrustumScale;
	cameraToClipMatrix[2].z = (fzFar + fzNear) / (fzNear - fzFar);
	cameraToClipMatrix[3].z = (2 * fzFar * fzNear) / (fzNear - fzFar);
	cameraToClipMatrix[2].w = -1.0f;

	glUseProgram(theProgram);
	glUniformMatrix4fv(cameraToClipMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
	glUseProgram(0);
	//deletes shaders after use
	//std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

const int numberOfVertices = 8;

#define GREEN_COLOR 0.75f, 0.75f, 1.0f, 1.0f
#define BLUE_COLOR 	0.0f, 0.5f, 0.0f, 1.0f
#define RED_COLOR 1.0f, 0.0f, 0.0f, 1.0f
#define GREY_COLOR 0.8f, 0.8f, 0.8f, 1.0f
#define BROWN_COLOR 0.5f, 0.5f, 0.0f, 1.0f

const float vertexData[] =
{
	+1.0f, +1.0f, +1.0f,
	-1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	+1.0f, +1.0f, -1.0f,
	+1.0f, -1.0f, +1.0f,
	-1.0f, +1.0f, +1.0f,

	GREEN_COLOR,
	BLUE_COLOR,
	RED_COLOR,
	BROWN_COLOR,

	GREEN_COLOR,
	BLUE_COLOR,
	RED_COLOR,
	BROWN_COLOR,
};

const GLshort indexData[] =
{
	0, 1, 2,
	1, 0, 3,
	2, 3, 0,
	3, 2, 1,

	5, 4, 6,
	4, 5, 7,
	7, 6, 4,
	6, 7, 5,
};

glm::vec3 StationaryOffset(float fElapsedTime)
{
	return glm::vec3(0.0f, 0.0f, -20.0f);
}

glm::vec3 OvalOffset(float fElapsedTime)
{
	const float fLoopDuration = 3.0f;
	const float fScale = 3.14159f * 2.0f / fLoopDuration;

	float fCurrTimeThroughLoop = fmodf(fElapsedTime, fLoopDuration);

	return glm::vec3(cosf(fCurrTimeThroughLoop * fScale) * 4.f,
		sinf(fCurrTimeThroughLoop * fScale) * 6.f,
		-20.0f);
}

glm::vec3 BottomCircleOffset(float fElapsedTime)
{
	const float fLoopDuration = 12.0f;
	const float fScale = 3.14159f * 2.0f / fLoopDuration;

	float fCurrTimeThroughLoop = fmodf(fElapsedTime, fLoopDuration);

	return glm::vec3(cosf(fCurrTimeThroughLoop * fScale) * 5.f,
		-3.5f,
		sinf(fCurrTimeThroughLoop * fScale) * 5.f - 20.0f);
}

struct Instance
{
	typedef glm::vec3(*OffsetFunc)(float);

	OffsetFunc CalcOffset;

	glm::mat4 ConstructMatrix(float fElapsedTime)
	{
		glm::mat4 theMat(1.0f);

		theMat[3] = glm::vec4(CalcOffset(fElapsedTime), 1.0f);

		return theMat;
	}
};

Instance g_instanceList[] =
{
	{ StationaryOffset },
	{ OvalOffset },
	{ BottomCircleOffset },
};

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeVertexBuffer();



	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	size_t colorDataOffset = sizeof(float) * 3 * numberOfVertices;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorDataOffset);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBindVertexArray(0);

	//enables face culling to not draw sides we cannot see
	glEnable(GL_CULL_FACE);
	//culls the back
	glCullFace(GL_BACK);
	//defines clockwise winding order to be the front
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
}

void InitializeVertexBuffer()
{
	glGenBuffers(1, &vertexBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBufferObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}






void reshape(int w, int h) {
	cameraToClipMatrix[0].x = fFrustumScale / (w / (float)h);
	cameraToClipMatrix[1].y = fFrustumScale;

	glUseProgram(theProgram);
	glUniformMatrix4fv(perspectiveMatrixUnif, 1, GL_FALSE, glm::value_ptr(cameraToClipMatrix));
	glUseProgram(0);

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void display() {

	//clears screen buffer using the colour black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(theProgram);

	glBindVertexArray(vao);

	float fElapsedTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	for (int iLoop = 0; iLoop < ARRAY_COUNT(g_instanceList); iLoop++)
	{
		Instance &currInst = g_instanceList[iLoop];
		const glm::mat4 &transformMatrix = currInst.ConstructMatrix(fElapsedTime);

		glUniformMatrix4fv(modelToCameraMatrixUnif, 1, GL_FALSE, glm::value_ptr(transformMatrix));
		glDrawElements(GL_TRIANGLES, ARRAY_COUNT(indexData), GL_UNSIGNED_SHORT, 0);
	}

	glBindVertexArray(0);
	glUseProgram(0);

	glutSwapBuffers();
	glutPostRedisplay();
}
  

void keyboard(unsigned char key, int x, int y)
{
	static bool bDepthClampingActive = false;
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;
	case 32:
		if (bDepthClampingActive)
    			glDisable(GL_DEPTH_CLAMP);
		else
			glEnable(GL_DEPTH_CLAMP);

		bDepthClampingActive = !bDepthClampingActive;
		break;
	}
}