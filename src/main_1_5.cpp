#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"

GLuint program;
Core::Shader_Loader shaderLoader;

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	glUseProgram(program);

	glm::mat4 rotation;
	
	// ZADANIE: Wyswietl obracajacy sie czworokat
	//
	// Uzyj zmiennej "time" zeby ustalic taka macierz rotacji, ktora powoduje obrot wokol poczatku ukladu wspolrzednych
	// Kat rotacji musi byc wyrazony w radianach
	
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&rotation);

	// Uzyj kodu do rysowania czworokata z poprzedniego zadania

	glUseProgram(0);
	glutSwapBuffers();
}

void init()
{
	program = shaderLoader.CreateProgram("shaders/shader_1_2.vert", "shaders/shader_1_2.frag");
}

void shutdown()
{
	shaderLoader.DeleteProgram(program);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Pierwszy Program");
	glewInit();

	init();
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
