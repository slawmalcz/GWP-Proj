#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"

GLuint program;
Core::Shader_Loader shaderLoader;

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	// Zmienna "time" po wykonaniu tej linii kodu zawiera liczbe sekund od uruchomienia programu
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	glUseProgram(program);

	// "translation" jest automatycznie inicjalizowane macierza jednostkowa 4 x 4
	glm::mat4 translation;
	
	// ZADANIE: Wyswietl poruszajacy sie czworokat
	//
	// Uzyj zmiennej "time" zeby ustalic taka macierz translacji, ktora powoduje przemieszczanie rysowanego obiektu pomiedzy gora i dolem ekranu (wartosci y maja byc pomiedzy -0.5 i 0.5)
	// Skorzystaj z funkcji fmodf(float x, float y) http://en.cppreference.com/w/c/numeric/math/fmod 
	
	// Polecenie glUniformMatrix4fv wysyla zmienna "translation" do GPU i przypisuje ja do zmiennej typu mat4 o nazwie "transformation" w shaderze.
	// Shader uzyje tej macierzy do transformacji wierzcholkow podczas rysowania.
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&translation);

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
