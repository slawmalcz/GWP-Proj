#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>


#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

//Parameters
#pragma region MenuItemsVariables
enum
{
	FULL_WINDOW, // aspekt obrazu - całe okno
	UNLOCK_CAMERA,
	ASPECT_1_1,// Unlocking camera from 
	EXIT // wyjście
};
int Aspect = FULL_WINDOW;
#pragma endregion
#pragma region ControlVariables
float angleSpeed = 0.1f;
float moveSpeed = 0.1f;
float AngleBorder = 0.2F;
bool cameraLock = true;
#pragma endregion
#pragma region GlobalVariables
GLuint programColor;
GLuint programTexture;
GLuint programSkyBox;

Core::Shader_Loader shaderLoader;

float appLoadingTime;
float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-5, 0, 0);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));
#pragma endregion
#pragma region Models
#pragma region Fish
	obj::Model fishCorpusModel;
	obj::Model fishHeadModel;
	obj::Model fishTailModel;
	GLuint fishHeadTexture;
	GLuint fishCorpusTexture;
	GLuint fishTailTexture;
#pragma endregion
obj::Model sandModel;
GLuint sandTexture;
obj::Model coralModel;
obj::Model skyBoxModel;
GLuint skyBoxTexture;
obj::Model stoneModel;
GLuint stoneTexture;
#pragma endregion
#pragma region CoralGeneratorParameters
int const CORALNUMBERS = 40;
glm::vec3 CoralsLocation [CORALNUMBERS];
glm::vec3 CoralsScale[CORALNUMBERS];
glm::vec3 CoralsColor[CORALNUMBERS];
glm::mat4 CoralsRotationMatrix[CORALNUMBERS];
#pragma endregion
#pragma region StoneGeneratorParameters
int const STONESNUMBER = 40;
glm::vec3 StonesLocation[STONESNUMBER];
glm::vec3 StonesScale[STONESNUMBER];
glm::mat4 StonesRotationMatrix[STONESNUMBER];
#pragma endregion

//Class
#pragma region Hierarchical Transformation
class HierarchicalMatrix {
	HierarchicalMatrix *parent;
	glm::mat4 transformationMatrix;

#pragma region Constructors
	public :
		HierarchicalMatrix() {
			parent = NULL;
			transformationMatrix = glm::mat4();
		}

		HierarchicalMatrix(glm::mat4 _transformationMatrix, HierarchicalMatrix *_parent) {
			transformationMatrix = _transformationMatrix;
			parent = _parent;
		}
#pragma endregion

#pragma region Setters and Getters
		HierarchicalMatrix * GetParent() {
			return parent;
		}
		void SetParent(HierarchicalMatrix* _newParent) {
			parent = _newParent;
		}
		glm::mat4 GetTransformationMatrix() {
			return transformationMatrix;
		}
		void SetTransformationMatrix(glm::mat4 _newTransformationMatrix) {
			transformationMatrix = _newTransformationMatrix;
		}
#pragma endregion

#pragma region Methods and functions
		glm::mat4 GetFinalTransformationMatrix() {
			HierarchicalMatrix *actualMatrix = this->GetParent();
			glm::mat4 finalTransformMatrix = this->GetTransformationMatrix();
			while (&actualMatrix == NULL) {
				finalTransformMatrix *= actualMatrix->GetTransformationMatrix();
				actualMatrix = actualMatrix->GetParent();
			}
			return finalTransformMatrix;
		}
#pragma endregion
};
#pragma endregion

//Functions
#pragma region AdditionalFunctions
glm::mat4 createCameraMatrix()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f - appLoadingTime;

	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0, 1, 0);

	return Core::createViewMatrix(cameraPos, cameraDir, up);
}
float GetTime() 
{
	return glutGet(GLUT_ELAPSED_TIME) / 1000.0f - appLoadingTime;
}
glm::mat4 ApplyWaveFunction(glm::vec3 rotationDirection, glm::vec3 pointOfRotation,float borderAngle = 0.2, float sinParamerer = 1)
{
	float angle = sinf(GetTime()*sinParamerer) * borderAngle;
	glm::mat4 retMatrix = glm::translate(-pointOfRotation)*glm::rotate(angle, rotationDirection)*glm::translate(pointOfRotation);
	return retMatrix;
}
void SetCoralsParameters() {
	for (int i = 0; i < CORALNUMBERS; i++) {
		CoralsLocation[i] = glm::vec3(rand()%50 -25, -1.5, rand() % 50 - 25);
		CoralsScale[i] = glm::vec3(rand() % 3 + 3, rand() % 3 + 3, rand() % 3 + 3);
		CoralsColor[i] = glm::vec3(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));

		float CoralsAngleX = ((double)rand() / (RAND_MAX));
		float CoralsAngleY = ((double)rand() / (RAND_MAX));

		CoralsRotationMatrix[i] = glm::rotate(CoralsAngleY, glm::vec3(1, 0, 0)) * glm::rotate(CoralsAngleX, glm::vec3(0, 1, 0));
	}
}
void SetStoneParameters() {
	for (int i = 0; i < STONESNUMBER; i++) {
		StonesLocation[i] = glm::vec3(rand() % 50 - 25, -1.5, rand() % 50 - 25);
		StonesScale[i] = glm::vec3(rand() % 3 + 3, rand() % 3 + 3, rand() % 3 + 3);

		float CoralsAngleX = ((double)rand() / (RAND_MAX));
		float CoralsAngleY = ((double)rand() / (RAND_MAX));

		StonesRotationMatrix[i] = glm::rotate(CoralsAngleY, glm::vec3(1, 0, 0)) * glm::rotate(CoralsAngleX, glm::vec3(0, 1, 0));
	}
}
#pragma endregion
#pragma region DrawFunctions
void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void drawObjectSkyBox(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programSkyBox;

	glUseProgram(program);

	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
#pragma endregion
#pragma region Drawing complex shapes
void GenerateFish() {
	glm::mat4 fishHeadMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0, -0.25f, 0)) /* glm::rotate(-cameraAngle, glm::vec3(0, 1, 0)) */ * glm::scale(glm::vec3(0.25f));
	drawObjectTexture(&fishHeadModel, fishHeadMatrix, fishHeadTexture);

	glm::mat4 fishCorpusMatrix = fishHeadMatrix * glm::translate(glm::vec3(-0.5782F, 0.012F, 0.0106F)) * ApplyWaveFunction(glm::vec3(0,1,0), glm::vec3(-0.5782F, 0.012F, 0.0106F));
	drawObjectTexture(&fishCorpusModel, fishCorpusMatrix, fishCorpusTexture);
	
	glm::mat4 fishTailMatrix = fishCorpusMatrix * glm::translate(glm::vec3(-0.5704F, 0.041F, 0)) * ApplyWaveFunction(glm::vec3(0, 1, 0), glm::vec3(-0.1, 0, 0),0.4,-1);
	drawObjectTexture(&fishTailModel, fishTailMatrix, fishTailTexture);
}

void GenerateCoral() {
	for (int i = 0; i < CORALNUMBERS; i++) {
		glm::mat4 coralModelMatrix1 = glm::translate(CoralsLocation[i]) * glm::scale(CoralsScale[i]) * CoralsRotationMatrix[i];
		drawObjectColor(&coralModel, coralModelMatrix1, CoralsColor[i]);
	}
}
void GenerateStones() {
	for (int i = 0; i < STONESNUMBER; i++) {
		glm::mat4 stoneModelMatrix = glm::translate(StonesLocation[i]) * glm::scale(StonesScale[i]) * StonesRotationMatrix[i];
		drawObjectTexture(&stoneModel, stoneModelMatrix, stoneTexture);
	}
}
#pragma endregion
#pragma region ProgramFlow
void Display()
{
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	//Background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);

	drawObjectSkyBox(&skyBoxModel, glm::mat4(), skyBoxTexture);

	//Fish generation
	GenerateFish();

	glm::mat4 sandModelMatrix = glm::translate(glm::vec3(0, -3.0f, 0));
	drawObjectTexture(&sandModel, sandModelMatrix, sandTexture);

	GenerateCoral();
	GenerateStones();

	glutSwapBuffers();
}
void Init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programSkyBox = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_texSky.frag");
	//Fish
	fishHeadModel = obj::loadModelFromFile("models/HeadFix.obj");
	fishCorpusModel = obj::loadModelFromFile("models/CorpusFix.obj");
	fishTailModel = obj::loadModelFromFile("models/TailFix.obj");

	fishHeadTexture = Core::LoadTexture("textures/HeadTexture.png");
	fishCorpusTexture = Core::LoadTexture("textures/CorpusTexture.png");
	fishTailTexture = Core::LoadTexture("textures/TailTexture.png");

	//Snad
	sandModel = obj::loadModelFromFile("models/Sand.obj");
	sandTexture = Core::LoadTexture("textures/Sand.png");

	//Coral
	coralModel = obj::loadModelFromFile("models/Coral.obj");
	//Stones
	skyBoxModel = obj::loadModelFromFile("models/SkyBox.obj");
	skyBoxTexture = Core::LoadTexture("textures/Skybox.png");

	//Skybox
	stoneModel = obj::loadModelFromFile("models/stone.obj");
	stoneTexture = Core::LoadTexture("textures/stone.png");

	appLoadingTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	SetCoralsParameters();
	SetStoneParameters();
}
void Shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}
void Idle()
{
	glutPostRedisplay();
}
#pragma endregion
#pragma region ReshapeFunctions
void Reshape(int width, int height)
{
	// obszar renderingu - całe okno
	glViewport(0, 0, width, height);

	// wybór macierzy rzutowania
	glMatrixMode(GL_PROJECTION);

	// macierz rzutowania = macierz jednostkowa
	glLoadIdentity();

	// parametry bryły obcinania
	if (Aspect == ASPECT_1_1)
	{
		// wysokość okna większa od wysokości okna
		if (width < height && width > 0)
			glFrustum(-2.0, 2.0, -2.0 * height / width, 2.0 * height / width, 1.0, 5.0);
		else

			// szerokość okna większa lub równa wysokości okna
			if (width >= height && height > 0)
				glFrustum(-2.0 * width / height, 2.0 * width / height, -2.0, 2.0, 1.0, 5.0);

	}
	else
		glFrustum(-2.0, 2.0, -2.0, 2.0, 1.0, 5.0);

	// generowanie sceny 3D
	Display();
}
#pragma endregion
#pragma region KeyFunctions
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q': cameraAngle -= angleSpeed; break;
	case 'e': cameraAngle += angleSpeed; break;
	}
	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}
void SpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:cameraPos += cameraDir * moveSpeed; break;
	case GLUT_KEY_DOWN:cameraPos -= cameraDir * moveSpeed; break;
	case GLUT_KEY_LEFT:cameraPos -= glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case GLUT_KEY_RIGHT:cameraPos += glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	}

	Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}
#pragma endregion
#pragma region MenuFunctions
void Menu(int value)
{
	switch (value)
	{
		// obszar renderingu - całe okno
	case FULL_WINDOW:
		Aspect = FULL_WINDOW;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		Aspect = ASPECT_1_1;
		Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// wyjście
	case EXIT:
		Shutdown();
		exit(0);
	case UNLOCK_CAMERA:
		cameraLock = !cameraLock;
	}
}
#pragma endregion

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("GRK-Proj");
	glewInit();

	Init();
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);

	glutCreateMenu(Menu);
	glutAddMenuEntry("Aspekt obrazu - cale okno", FULL_WINDOW);
	glutAddMenuEntry("Aspekt obrazu 1:1", ASPECT_1_1);
	glutAddMenuEntry("Wyjscie", EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	Shutdown();
	return 0;
}