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
glm::vec3 currentLocation;
#pragma endregion
#pragma region GlobalVariables
GLuint programColor;
GLuint programTexture;
GLuint programSkyBox;
GLuint programNormal;
GLuint programBubble;

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
	GLuint fishDiffuseTexture;
	GLuint fishNormalTexture;
#pragma endregion
obj::Model sandModel;
GLuint sandTexture;
GLuint snadNormal;

obj::Model coralModel;

obj::Model skyBoxModel;
GLuint skyBoxTexture;

obj::Model stoneModel;
GLuint stoneTexture;

obj::Model bubbleModel;
GLuint bubbleTexture;

obj::Model rockModel;
GLuint rockTexture;
GLuint rockNormal;
#pragma endregion
#pragma region CoralGeneratorParameters
int const CORALNUMBERS = 80;
glm::vec3 CoralsLocation [CORALNUMBERS];
glm::vec3 CoralsScale[CORALNUMBERS];
glm::vec3 CoralsColor[CORALNUMBERS];
glm::mat4 CoralsRotationMatrix[CORALNUMBERS];
#pragma endregion
#pragma region StoneGeneratorParameters
int const STONESNUMBER = 80;
glm::vec3 StonesLocation[STONESNUMBER];
glm::vec3 StonesScale[STONESNUMBER];
glm::mat4 StonesRotationMatrix[STONESNUMBER];
#pragma endregion
#pragma region BubbleGeneratorParameters
int const BUBBLESNUMBER = 200;
glm::vec3 BubblesLocation[BUBBLESNUMBER];
glm::vec3 BubblesScale[BUBBLESNUMBER];
glm::mat4 BubblesRotationMatrix[BUBBLESNUMBER];
float BubblesLifeSpane[BUBBLESNUMBER];
glm::vec3 BubbleSeed[BUBBLESNUMBER];
#pragma endregion
#pragma region RockGeneratorParameters
int const ROCKNUMBER = 10;
glm::vec3 RocksLocation[ROCKNUMBER];
glm::vec3 RocksScale[ROCKNUMBER];
glm::mat4 RocksRotationMatrix[ROCKNUMBER];
#pragma endregion
#pragma region FishesGenerator
int const NUMOFFISHES = 20;
int const NUMOFINTRESTPOINT = 10;
glm::vec3 IntrestPoint[20][10];
float currentTimelaps;
int currentPointOfIntrest;
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
#pragma region CollisionFunctions
bool CollisionSkyBox(glm::vec3 _currentLocationDeviation) {
	if (pow(_currentLocationDeviation.x, 2) + pow(_currentLocationDeviation.z, 2) < pow(23, 2)) {
		return false;
	}
	else {
		return true;
	}
}
bool CheckCollision(glm::vec3 _currentLocation, glm::vec3 _currentLocationDeviation) {
	if (CollisionSkyBox(_currentLocationDeviation))return true;
	return false;
}
#pragma endregion
#pragma region AdditionalFunctions
glm::vec3 catmullRomCurvature(float t,int object, int pi_1, int pi, int pi1, int pi2) {
	glm::vec3 ret = 0.5*(
		glm::vec4(pow(t, 3), pow(t, 2), t, 1) *
		glm::mat4(-1, 3, -3, 1,
			2, -5, 4, -1,
			-1, 0, 1, 0,
			0, 2, 0, 0) *
		glm::mat4x3(IntrestPoint[object][pi_1], IntrestPoint[object][pi], IntrestPoint[object][pi1], IntrestPoint[object][pi2]));
	return ret;
}
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
glm::vec3 UseOfCatMullRom(int object) {
	int pi_1 = currentPointOfIntrest - 1;
	int pi = currentPointOfIntrest;
	int pi1 = currentPointOfIntrest + 1;
	int pi2 = currentPointOfIntrest + 2;

	if (pi_1 < 0)pi_1 = NUMOFINTRESTPOINT - 1;
	if (pi1 >= NUMOFINTRESTPOINT) pi1 = pi1 % NUMOFINTRESTPOINT;
	if (pi2 >= NUMOFINTRESTPOINT) pi2 = pi2 % NUMOFINTRESTPOINT;

	return catmullRomCurvature(currentTimelaps, object, pi_1, pi, pi1, pi2);
}
void SetCoralsParameters() {
	for (int i = 0; i < CORALNUMBERS; i++) {
		CoralsLocation[i] = glm::vec3(rand()%50 -25, -4, rand() % 50 - 25);
		CoralsScale[i] = glm::vec3(rand() % 3 + 3, rand() % 3 + 3, rand() % 3 + 3);
		CoralsColor[i] = glm::vec3(((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)));

		float CoralsAngleX = ((double)rand() / (RAND_MAX));
		float CoralsAngleY = ((double)rand() / (RAND_MAX));

		CoralsRotationMatrix[i] = glm::rotate(CoralsAngleY, glm::vec3(1, 0, 0)) * glm::rotate(CoralsAngleX, glm::vec3(0, 1, 0));
	}
}
void SetStoneParameters() {
	for (int i = 0; i < STONESNUMBER; i++) {
		StonesLocation[i] = glm::vec3(rand() % 50 - 25, -4, rand() % 50 - 25);
		StonesScale[i] = glm::vec3(rand() % 3 + 3, rand() % 3 + 3, rand() % 3 + 3);

		float CoralsAngleX = ((double)rand() / (RAND_MAX));
		float CoralsAngleY = ((double)rand() / (RAND_MAX));

		StonesRotationMatrix[i] = glm::rotate(CoralsAngleY, glm::vec3(1, 0, 0)) * glm::rotate(CoralsAngleX, glm::vec3(0, 1, 0));
	}
}
void SetRocksParameters() {
	for (int i = 0; i < ROCKNUMBER; i++) {
		RocksLocation[i] = glm::vec3(rand() % 50 - 25, -4, rand() % 50 - 25);
		RocksScale[i] = glm::vec3(1,1,1);

		float CoralsAngleX = ((double)rand() / (RAND_MAX));
		float CoralsAngleY = ((double)rand() / (RAND_MAX));

		RocksRotationMatrix[i] = glm::rotate(CoralsAngleY, glm::vec3(1, 0, 0)) * glm::rotate(CoralsAngleX, glm::vec3(0, 1, 0));
	}
}
void SetBubbleParameters() {
	for (int i = 0; i < BUBBLESNUMBER; i++) {
		BubblesLocation[i] = glm::vec3(rand() % 50 - 25, rand() % 20 - 25, rand() % 50 - 25);
		BubblesScale[i] = glm::vec3(rand() % 7+1);
		BubbleSeed[i] = glm::vec3(rand() % 5, rand() % 5,rand() % 5);

		float BubblesAngleX = ((double)rand() / (RAND_MAX));
		float BubblesAngleY = ((double)rand() / (RAND_MAX));
		float BubblesAmgleZ = ((double)rand() / (RAND_MAX));
		BubblesLifeSpane[i] = GetTime();

		BubblesRotationMatrix[i] = glm::rotate(BubblesAngleY, glm::vec3(1, 0, 0)) * glm::rotate(BubblesAngleX, glm::vec3(0, 1, 0)) * glm::rotate(BubblesAmgleZ, glm::vec3(0,0,1));
	}
}
void SetFishesParameters() {
	for (int i = 0; i < NUMOFFISHES; i++) {
		for (int l = 0; l < NUMOFINTRESTPOINT; l++) {
			IntrestPoint[i][l] = glm::vec3(rand() % 50 - 25, rand() % 50 - 25, rand() % 50 - 25);
		}
	}
	currentTimelaps = 0;
	currentPointOfIntrest = 0;
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
void drawObjectTextureWithNormal(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId, GLuint normalId)
{
	GLuint program = programNormal;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(normalId, "normalSampler", program, 1);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void drawObjectBubble(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programBubble;

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
	glm::mat4 fishHeadMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0, -0.25f, 0)) * glm::rotate(-cameraAngle, glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));
	drawObjectTextureWithNormal(&fishHeadModel, fishHeadMatrix, fishDiffuseTexture,fishNormalTexture);

	glm::mat4 fishCorpusMatrix = fishHeadMatrix * ApplyWaveFunction(glm::vec3(0,1,0), glm::vec3(-0.5782F, 0.012F, 0.0106F));
	drawObjectTextureWithNormal(&fishCorpusModel, fishCorpusMatrix, fishDiffuseTexture, fishNormalTexture);
	
	glm::mat4 fishTailMatrix = fishCorpusMatrix;//* (glm::vec3(0, 1, 0), glm::vec3(-0.1, 0, 0),0.4,-1);
	drawObjectTextureWithNormal(&fishTailModel, fishTailMatrix, fishDiffuseTexture, fishNormalTexture);
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
void GenerateRocks() {
	for (int i = 0; i < ROCKNUMBER; i++) {
		glm::mat4 stoneModelMatrix = glm::translate(RocksLocation[i]) * glm::scale(RocksScale[i]) * RocksRotationMatrix[i];
		drawObjectTextureWithNormal(&rockModel, stoneModelMatrix, rockTexture,rockNormal);
	}
}
void GenerateBubbles() {
	for (int i = 0; i < BUBBLESNUMBER; i++) {
		if (BubblesLocation[i].y +(GetTime() - BubblesLifeSpane[i])*BubbleSeed[i].x > 25) BubblesLifeSpane[i] = GetTime();
		glm::mat4 bubbleModelMatrix = 
			glm::translate(
				glm::vec3(
					BubblesLocation[i].x + sinf(GetTime() * BubbleSeed[i].y), 
					BubblesLocation[i].y + (GetTime() - BubblesLifeSpane[i]) * BubbleSeed[i].x,
					BubblesLocation[i].z + cosf(GetTime() * BubbleSeed[i].z)
				)) * glm::scale(BubblesScale[i]) * BubblesRotationMatrix[i];
		drawObjectBubble(&bubbleModel, bubbleModelMatrix, bubbleTexture);
	}
}
void GenerateFishes() {
	for (int i = 0; i < NUMOFFISHES; i++) {
		glm::mat4 tempTransform = glm::translate(UseOfCatMullRom(i));
		//TODO:: UseTo draw a fish
		drawObjectTexture(&fishTailModel,tempTransform,fishDiffuseTexture);
	}

	if (currentTimelaps + 0.2 >= 1) {
		currentTimelaps = 0;
		if (currentPointOfIntrest + 1 >= NUMOFINTRESTPOINT) {
			currentPointOfIntrest = 0;
		}
		else {
			currentPointOfIntrest++;
		}
	}
	else {
		currentTimelaps = +0.1;
	}
}
void GenerateSkyBox() {
	drawObjectSkyBox(&skyBoxModel, glm::mat4(), skyBoxTexture);
}
void GenerateSand() {
	glm::mat4 sandModelMatrix = glm::translate(glm::vec3(0, -3.0f, 0)) * glm::scale(glm::vec3(3, 3, 3));
	drawObjectTextureWithNormal(&sandModel, sandModelMatrix, sandTexture, snadNormal);
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

	GenerateSkyBox();
	GenerateFish();
	GenerateSand();
	GenerateCoral();
	GenerateStones();
	GenerateBubbles();
	GenerateRocks();
	//GenerateFishes();

	glutSwapBuffers();
}
void Init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");
	programSkyBox = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_texSky.frag");
	programNormal = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_nor.frag");
	programBubble = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_bub.frag");
	//Fish
	fishHeadModel = obj::loadModelFromFile("models/HeadFix.obj");
	fishCorpusModel = obj::loadModelFromFile("models/CorpusFix.obj");
	fishTailModel = obj::loadModelFromFile("models/TailFix.obj");

	fishDiffuseTexture = Core::LoadTexture("textures/fish_diffuse.png");
	fishNormalTexture = Core::LoadTexture("textures/fish_normal.png");

	//Snad
	sandModel = obj::loadModelFromFile("models/Underwater.obj");
	sandTexture = Core::LoadTexture("textures/Sand.png");
	snadNormal = Core::LoadTexture("textures/NormalMapUnderwater.png");

	//Coral
	coralModel = obj::loadModelFromFile("models/Coral.obj");

	//Skybox
	skyBoxModel = obj::loadModelFromFile("models/SkyBox.obj");
	skyBoxTexture = Core::LoadTexture("textures/Skybox.png");

	//Stones
	stoneModel = obj::loadModelFromFile("models/stone.obj");
	stoneTexture = Core::LoadTexture("textures/stone.png");

	//Bubble
	bubbleModel = obj::loadModelFromFile("models/Bubble.obj");
	bubbleTexture = Core::LoadTexture("textures/Bubble.png");

	//Rock
	rockModel = obj::loadModelFromFile("models/Rock.obj");
	rockTexture = Core::LoadTexture("textures/Rock.png");
	rockNormal = Core::LoadTexture("textures/NormalMapRock.png");

	appLoadingTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	SetCoralsParameters();
	SetStoneParameters();
	SetRocksParameters();
	SetBubbleParameters();
	SetFishesParameters();
	currentLocation = glm::vec3(0, 0, 0);
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
	glm::vec3 currentLocationDeviation = currentLocation + cameraDir * moveSpeed;
	switch (key)
	{
	case GLUT_KEY_UP:
		currentLocationDeviation = currentLocation + cameraDir * moveSpeed;
		if (!CheckCollision(currentLocation, currentLocationDeviation)) {
			cameraPos += cameraDir * moveSpeed;
		}
		break;
	case GLUT_KEY_DOWN:
		currentLocationDeviation = currentLocation - cameraDir * moveSpeed;
		if (!CheckCollision(currentLocation, currentLocationDeviation)) {
			cameraPos -= cameraDir * moveSpeed;
		}
		break;
	case GLUT_KEY_LEFT:
		currentLocationDeviation = currentLocation - glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed;;
		if (!CheckCollision(currentLocation, currentLocationDeviation)) {
			cameraPos -= glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed;
		}
		break;
	case GLUT_KEY_RIGHT:
		currentLocationDeviation = currentLocation + glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed;;
		if (!CheckCollision(currentLocation, currentLocationDeviation)) {
			cameraPos += glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed;
		}
		break;
	}
	currentLocation = cameraPos;

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