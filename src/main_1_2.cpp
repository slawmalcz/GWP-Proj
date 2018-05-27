#include "glew.h"
#include "freeglut.h"

#include "Shader_Loader.h"
#include "Render_Utils.h"

GLuint program; // Identyfikator shadera uzywanego do rysowania
Core::Shader_Loader shaderLoader;

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	
	// Aktywowanie shadera
	glUseProgram(program);
	
	// ZADANIE: Stworz trojkat z trzech wierzcholkow (typu const float []) i narysuj go za pomoca funkcji Core::DrawVertexArray z Render_Utils.cpp
	// Shader oczekuje pozycji opisanych 4 liczbami typu float (x, y, z, w).

	// Wylaczenie shadera
	glUseProgram(0);

	glutSwapBuffers();
}

void init()
{
	program = shaderLoader.CreateProgram("shaders/shader_1_1.vert", "shaders/shader_1_1.frag");
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
