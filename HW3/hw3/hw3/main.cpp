#define GLM_ENABLE_EXPERIMENTAL

#include "Object.h"
#include "FreeImage.h"
#include "glew.h"
#include "freeglut.h"
#include "shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"
#include <iostream>
#include <string>
#include <math.h>
#include <stb_image.h>
#include <ctime>
#include <chrono>
#include <thread>
#include "Vertex.h"
#include <vector>

using namespace std;

void shaderInit();
void bindbufferInit();
void textureInit();
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void DrawLightPoint();
void DrawModel();
void LoadTexture(unsigned int&, const char*);
void Sleep(int ms);

GLuint program,Phongprogram, Toonprogram;
GLuint VAO, VBO;
unsigned int ToonTexture, modeltexture;
float windowSize[2] = { 600, 600 };
float angle = 0.0f;
glm::vec3 WorldLightPos = glm::vec3(2, 5, 5);
glm::vec3 WorldLightStep = glm::vec3(2, 5, 5) / 5.0f;
glm::vec3 WorldCamPos = glm::vec3(7.5, 5.0, 7.5);
bool EdgeFlag = false;

// feeling free to adjust below value to fit your computer efficacy.
#define Rotate_Speed 1
#define MAX_FPS 120
// timer for FPS control
clock_t Start, End;
// the speed of rotation
float speed = 0;

//Object* model = new Object("Umbreon.obj");
Object* model = new Object("Pikachu.obj");

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("hw3");

	glewInit();
	shaderInit();
	bindbufferInit();
	textureInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit() {
	GLuint vert = createShader("Shaders/Phongshading.vert", "vertex");
	GLuint frag = createShader("Shaders/Phongshading.frag", "fragment");
	Phongprogram = createProgram(vert, frag);

	vert = createShader("Shaders/Toon.vert", "vertex");
	frag = createShader("Shaders/Toon.frag", "fragment");
	Toonprogram = createProgram(vert, frag);

	program = Phongprogram;
}

void bindbufferInit() {

	vector<VertexAttribute> data;
	VertexAttribute temp;
	for (int i = 0; i < model->positions.size() / 3; i++) {
		int idx = i * 3;
		Vertex pos(model->positions[idx], model->positions[idx + 1], model->positions[idx + 2]);
		temp.setPosition(pos);
		Vertex norm(model->normals[idx], model->normals[idx + 1], model->normals[idx + 2]);
		temp.setNormal(norm);
		idx = i * 2;
		temp.setTexcoord(model->texcoords[idx], model->texcoords[idx + 1]);
		data.push_back(temp);
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * data.size(), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)offsetof(VertexAttribute,position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)offsetof(VertexAttribute, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)offsetof(VertexAttribute, texcoord));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void textureInit() {
	//LoadTexture(modeltexture, "Umbreon.jpg");
	LoadTexture(modeltexture, "Pikachu.png");
}

glm::mat4 getV()
{
	// set camera position and configuration
	return glm::lookAt(glm::vec3(WorldCamPos.x, WorldCamPos.y, WorldCamPos.z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

glm::mat4 getP()
{
	// set perspective view
	float fov = 45.0f;
	float aspect = windowSize[0] / windowSize[1];
	float nearDistance = 1.0f;
	float farDistance = 1000.0f;
	return glm::perspective(glm::radians(fov), aspect, nearDistance, farDistance);
}

void display() {
	Start = clock();
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawLightPoint();
	DrawModel();
	angle += speed;
	End = clock();
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void LoadTexture(unsigned int& texture, const char* tFileName) {
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(tFileName, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'a':
		{
			glm::mat4 trans = glm::mat4(1.0f);
			trans = glm::rotate(trans, glm::radians(-10.0f), glm::vec3(0, 1, 0));
			glm::vec3 Zoomin = glm::normalize (-WorldLightPos);
			glm::vec4 Left = trans * glm::vec4(WorldLightPos,1.0);
			WorldLightPos = glm::vec3(Left.x,Left.y,Left.z);
			glm::vec4 new_step = trans * glm::vec4(WorldLightStep, 1.0);
			WorldLightStep = glm::vec3(new_step.x, new_step.y, new_step.z);
			break;
		}
		case 'd':
		{
			glm::mat4 trans = glm::mat4(1.0f);
			trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(0, 1, 0));
			glm::vec3 Zoomin = glm::normalize(-WorldLightPos);
			glm::vec4 Right = trans * glm::vec4(WorldLightPos, 1.0);
			WorldLightPos = glm::vec3(Right.x, Right.y, Right.z);
			glm::vec4 new_step = trans * glm::vec4(WorldLightStep, 1.0);
			WorldLightStep = glm::vec3(new_step.x, new_step.y, new_step.z);
			break;
		}
		// zoom in
		case 's':
		{
			//if (WorldLightPos.y <= 0) {
			//	break;
			//}
			WorldLightPos -= WorldLightStep;
			break;
		}
		// zoom out
		case 'w':
		{
			//if (WorldLightPos.y <= 0) {
			//	break;
			//}
			WorldLightPos += WorldLightStep;
			break;
		}
		// reset light position
		case 'r':
		{
			WorldLightPos = glm::vec3(2, 5, 5);
			break;
		}
		case 'p':
		{
			if (speed > 0) {
				speed = 0;
			}
			else {
				speed = Rotate_Speed;
			}
			break;
		}
		case '1':
		{
			program = Phongprogram;
			break;
		}
		case '2':
		{
			program = Toonprogram;
			break;
		}
		case '3':
		{
			EdgeFlag = !EdgeFlag;
			break;
		}
		default:
		{
			break;
		}
	}
}

void idle() {
	// FPS control
	clock_t CostTime = End - Start;
	float PerFrameTime = 1000.0 / MAX_FPS;
	if (CostTime < PerFrameTime) {
		Sleep(ceil(PerFrameTime) - CostTime);
	}
	glutPostRedisplay();
}

void DrawModel()
{
	glUseProgram(program);

	glm::mat4 M(1.0f);
	M = glm::translate(M, glm::vec3(0, 0, 0));
	M = glm::rotate(M, glm::radians(angle), glm::vec3(0, 1, 0));
	//M = glm::rotate(M, glm::radians(90.0f), glm::vec3(1, 0, 0));
	M = glm::scale (M, glm::vec3(5, 5, 5));

	GLuint ModelMatrixID = glGetUniformLocation(program, "M");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &M[0][0]);

	glm::mat4 V = getV();
	ModelMatrixID = glGetUniformLocation(program, "V");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &V[0][0]);

	glm::mat4 P = getP();
	ModelMatrixID = glGetUniformLocation(program, "P");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &P[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, modeltexture);
	glUniform1i(glGetUniformLocation(program, "texture"), 0);

	GLuint LightPosID = glGetUniformLocation(program, "WorldLightPos");
	glUniform3f(LightPosID, WorldLightPos.x, WorldLightPos.y, WorldLightPos.z);
		
	GLuint CameraPosID = glGetUniformLocation(program, "WorldCamPos");
	glUniform3f(CameraPosID, WorldCamPos.x, WorldCamPos.y, WorldCamPos.z);
	
	GLuint KaID = glGetUniformLocation(program, "Ka");
	glUniform3f(KaID, 1.0, 1.0, 1.0);

	GLuint KdID = glGetUniformLocation(program, "Kd");
	glUniform3f(KdID, 1.0, 1.0, 1.0);

	GLuint KsID = glGetUniformLocation(program, "Ks");
	glUniform3f(KsID, 1.0, 1.0, 1.0);

	GLuint LaID = glGetUniformLocation(program, "La");
	glUniform3f(LaID, 0.2, 0.2, 0.2);

	GLuint LdID = glGetUniformLocation(program, "Ld");
	glUniform3f(LdID, 0.8, 0.8, 0.8);

	GLuint LsID = glGetUniformLocation(program, "Ls");
	glUniform3f(LsID, 0.5, 0.5, 0.5);

	GLuint gloss = glGetUniformLocation(program, "gloss");
	glUniform1i(gloss, 100);

	GLuint EdgeFlagID = glGetUniformLocation(program, "EdgeFlag");
	glUniform1i(EdgeFlagID, EdgeFlag);

	glBindVertexArray(VAO);
	//glDrawArrays(GL_QUADS, 0, 4 * model->fNum);
	glDrawArrays(GL_TRIANGLES, 0, 3 * model->fNum);
	glBindVertexArray(0);
	glActiveTexture(0);
	glUseProgram(0);
}

void DrawLightPoint() {
	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], 1.0, 1000.0);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(WorldCamPos.x, WorldCamPos.y, WorldCamPos.z,// eye
		0.0, 0.0, 0.0,     // center
		0.0, 1.0, 0.0);    // up
	glPushMatrix();
	glTranslatef(WorldLightPos.x, WorldLightPos.y,WorldLightPos.z);
	glColor3f(1.0f, 1.0f, 0.0f);           
	glutSolidSphere(0.5f, 50.0f, 50.0f);    
	glPopMatrix();
}

void Sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}