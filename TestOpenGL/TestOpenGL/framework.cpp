#include "stdafx.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "framework.h"
#include "TestOpenGL.h"

namespace Framework
{

	std::string LoadFile(std::string path) {

		std::ifstream t(path);
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();

	}

	GLuint LoadShader(GLenum eShaderType, std::string & strShaderFile)
	{
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
			switch (eShaderType) {
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

}


int main(int argc, char** argv) {

	// initializing freeglut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(300, 200);
	glutCreateWindow("Test");

	//intializing glewInit
	GLenum err = glewInit();

	//initalizing programs
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}

