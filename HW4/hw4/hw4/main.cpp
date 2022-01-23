#define GLM_ENABLE_EXPERIMENTAL
#define _USE_MATH_DEFINES

#include "Object.h"
#include "FreeImage.h"
#include "glew.h"
#include "freeglut.h"
#include "shader.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
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

void keyboard(unsigned char key, int x, int y);
void shaderInit();
void bufferModel(Object* model, GLuint* vao, GLuint* vbo);
//
void bufferBall(vector<VertexAttribute> model, GLuint* vao, GLuint* vbo);

void bindbufferInit();
void textureInit();
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void DrawModel(Object* model,GLuint program, GLuint vao, GLuint texture_, glm::mat4 &M, GLenum DrawingMode);
void LoadTexture(GLuint& texture, const char* tFileName, GLuint* texture_id);
void Sleep(int ms);
glm::mat4 getV();
glm::mat4 getP();
void demo();
//
void DrawSphere(float radius, float slice, float stack);
void DisplaySphere(glm::mat4& M);

GLuint Dissolveprogram, Modelprogram, Erect3program, Frag3program, Erect4program, Frag4program;
//
GLuint Explosionprogram, Fireprogram;

GLuint Umbreon_VAO, Umbreon_VBO;
GLuint Umbreon_texture_ID;
GLuint UmbreonTexture;

GLuint Eevee_VAO, Eevee_VBO;
GLuint Eevee_texture_ID;

//
GLuint Ball_VAO, Ball_VBO;
GLuint Ball_texture_ID;
GLuint BallTexture;


float windowSize[2] = { 600, 600 };
float angle = 0.0f;

///////
float degree = 0.0f;
float displace = 0.0f;
bool collapsed = false;
bool dissolved = false;
float time_ = 0.0f;

glm::vec3 WorldLightPos = glm::vec3(2, 5, 5);
glm::vec3 WorldCamPos = glm::vec3(7.5, 5.0, 7.5);

// feeling free to adjust below value to fit your computer efficacy.
#define MAX_FPS 120
// timer for FPS control
clock_t Start, End;

Object* Umbreon = new Object("Umbreon.obj");
Object* Eevee = new Object("Eevee.obj");
//
vector<VertexAttribute> ball;

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("hw4");

	//
	DrawSphere(3, 60, 60);

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

// ### hint
// comment or delete the "demo" function & feel free to chage the content of keyboard function & control parameter

//control parameter
#define Effect_TIME 240
bool rotate_f = false;
bool model_f = true;
bool erect_f = false;
bool frag_f = false;
bool diss_f = false;
bool explo_f = false;
bool who = true;
unsigned int sec = 0;

void keyboard(unsigned char key, int x, int y) {
	model_f = true;
	switch (key) {
	// disable all effect
	case '1':
	{
		frag_f = false;
		erect_f = false;
		diss_f = false;
		explo_f = false;
		degree = 0.0;
		displace = 0.0;
		collapsed = false;
		break;
	}
	case '5':
	{
		sec = 0;
		frag_f = false;
		erect_f = false;
		diss_f = false;
		explo_f = true;
		break;
	
	}
	default:
	{
		break;
	}
	}
}

void shaderInit() {
	// ### hint
	// create your shader in here
	// below two shader "expand3" & "expand4" only differ in "input type of primitive" of geometry shader (one for triangle , one for quad)
	// what kind of primitive is sent to geometry shader is depend on what model you call in "glDrawArrays" function
	GLuint vert;
	GLuint goem;
	GLuint frag;
	// remeber to use program before passing value to uniformal variable
	// I use a flag in geometry shader to create different effect
	vert = createShader("Shaders/dissolve.vert", "vertex");
	frag = createShader("Shaders/dissolve.frag", "fragment");
	Dissolveprogram = createProgram(vert, 0, frag);

	vert = createShader("Shaders/explosion.vert", "vertex");
	goem = createShader("Shaders/explosion.geom", "geometry");
	frag = createShader("Shaders/explosion.frag", "fragment");
	Explosionprogram = createProgram(vert, goem, frag);

	vert = createShader("Shaders/fire.vert", "vertex");
	frag = createShader("Shaders/fire.frag", "fragment");
	Fireprogram = createProgram(vert, frag);

	vert = createShader("Shaders/model.vert", "vertex");
	frag = createShader("Shaders/model.frag", "fragment");
	Modelprogram = createProgram(vert, frag);

}

void bufferModel(Object* model, GLuint* vao, GLuint* vbo) {
	// ### hint
	// you can add what you need in "Object.h" & "Vertex.h" & buffer those data here
	vector<VertexAttribute> data;
	VertexAttribute temp;
	model->max_y = INT_MIN;
	model->min_y = INT_MAX;
	model->max_z = INT_MIN;
	model->min_z = INT_MAX;
	for (int i = 0; i < model->positions.size() / 3; i++) {
		int idx = i * 3;
		Vertex pos(model->positions[idx], model->positions[idx + 1], model->positions[idx + 2]);
		temp.setPosition(pos);
		Vertex norm(model->normals[idx], model->normals[idx + 1], model->normals[idx + 2]);
		temp.setNormal(norm);
		idx = i * 2;
		temp.setTexcoord(model->texcoords[idx], model->texcoords[idx + 1]);
		data.push_back(temp);
		model->max_y = max(model->max_y, model->positions[idx + 1]);
		model->min_y = min(model->min_y, model->positions[idx + 1]);
		model->max_z = max(model->max_z, model->positions[idx + 2]);
		model->min_z = min(model->min_z, model->positions[idx + 2]);
	}

	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);
	glBindVertexArray(*vao);

	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * data.size(), &data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)offsetof(VertexAttribute, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)offsetof(VertexAttribute, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)offsetof(VertexAttribute, texcoord));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void bufferBall(vector<VertexAttribute> model, GLuint* vao, GLuint* vbo) {
	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);
	glBindVertexArray(*vao);

	glBindBuffer(GL_ARRAY_BUFFER, *vbo);

	VertexAttribute* vertices;
	vertices = model.data();
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * model.size(), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}


// ### hint
// load any object model you want with texture below
// be careful of some obect model may need ".mtl file" which file name shouldn't be changed and the size of model & model's default orientaion may be different.
// it's not all of the model compoesd by triangle. you can know how to draw the model by drop the object file in to visual studio & you can see what kind of polygon makes up the model
// this demo using the two kind of model. one is "Eevee" composed of triangle another one is "Umbreon" composed of quads. (by the way Pikachu is also composed of triangle)
void bindbufferInit() {
	bufferModel(Umbreon, &Umbreon_VAO, &Umbreon_VBO);
	bufferModel(Eevee, &Eevee_VAO, &Eevee_VBO);
	bufferBall(ball, &Ball_VAO, &Ball_VBO);
}

void textureInit() {
	glEnable(GL_TEXTURE_2D); // not sure if this is needed
	LoadTexture(UmbreonTexture, "Umbreon.jpg", &Umbreon_texture_ID);
	LoadTexture(UmbreonTexture, "Eevee.jpg", &Eevee_texture_ID);
	LoadTexture(BallTexture, "Ball.jpg", &Ball_texture_ID);
}

void display() {
	Start = clock();
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	demo();
	End = clock();
	sec++;
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void LoadTexture(GLuint& texture, const char* tFileName, GLuint* texture_id) {
	static GLuint idx = 0;
	*texture_id = idx;
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + idx++);
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
	stbi_image_free(data);
}

void idle() {
	// FPS control
	clock_t CostTime = End - Start;
	float PerFrameTime = 2000.0 / MAX_FPS;
	if (CostTime < PerFrameTime) {
		Sleep(ceil(PerFrameTime) - CostTime);
	}
	if (displace >= 2) {
		collapsed = true;
	}
	glutPostRedisplay();

}

void Sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
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

// ### hint
// Reminding again : Eevee model & Umbreon model are composed of different polygon . (Don't forget Pikachu which is also provided in this homework. you can ues it.)
void demo() {
	// basic transformation matrix apply on all models
	glm::mat4 base_M(1.0f);
	// model matrix & transformation function to control model position
	glm::mat4 M2(base_M);
	M2 = glm::translate(M2, glm::vec3(0, -0.8, 0));
	//M2 = glm::rotate(M2, glm::radians(angle), glm::vec3(0, 1, 0));
	M2 = glm::rotate(M2, glm::radians(90.0f), glm::vec3(1, 0, 0));
	M2 = glm::scale(M2, glm::vec3(0.5, 0.5, 0.5));

	// some parameter I defined to contorl the shader result 
	// be careful of the different between Eevee & Umbreon
	//one using the z value another one using the y value beacuse of the default orientaion of model is different
	GLfloat Eevee_effect_t = (Eevee->max_y - Eevee->min_y) / Effect_TIME;
	glm::vec4 back_line = glm::vec4(0, Eevee->min_y, 0, 1);
	glm::vec4 base_line_normal = glm::vec4(0, 1, 0, 0);
	glm::vec4 front_line = glm::vec4(0, Eevee->max_y, 0, 1);
	back_line.y += Eevee_effect_t * sec;

	glm::vec4 back_line2(0, 0, Umbreon->min_z, 1);
	glm::vec4 front_line2(0, 0, Umbreon->max_z, 1);
	glm::vec4 base_line_normal2(0, 0, 1, 0);
	GLfloat Umbreon_effect_t = (Umbreon->max_z - Umbreon->min_z) / Effect_TIME;
	back_line2.z += Umbreon_effect_t * sec;
	if (explo_f)
	{
		//draw moon fairy
		glm::mat4 M1(base_M);
		M1 = glm::translate(M1, glm::vec3(-5, -5, 0));
		M1 = glm::rotate(M1, glm::radians(180.0f), glm::vec3(0, 1, 0));
		//DrawModel(Umbreon, Modelprogram, Umbreon_VAO, Umbreon_texture_ID, M1, GL_QUADS);
		if (dissolved == false) {
			if (back_line.z < Umbreon->max_z) {
			}
			else {
				// show the whole model after effect end
				back_line = glm::vec4(0, 0, Umbreon->min_z, 1);
			}
			glUseProgram(Dissolveprogram);
			GLfloat vecID = glGetUniformLocation(Dissolveprogram, "back_line");
			glUniform4fv(vecID, 1, &back_line2[0]);
			vecID = glGetUniformLocation(Dissolveprogram, "base_line_normal");
			glUniform4fv(vecID, 1, glm::value_ptr(base_line_normal2));
			vecID = glGetUniformLocation(Dissolveprogram, "front_line");
			glUniform4fv(vecID, 1, &front_line2[0]);

			glUseProgram(0);
			DrawModel(Umbreon, Dissolveprogram, Umbreon_VAO, Umbreon_texture_ID, M1, GL_QUADS);
		}

		// draw eevvee
		M2 = glm::translate(M2, glm::vec3(2, -5, 0));
		M2 = glm::scale(M2, glm::vec3(0.25, 0.25, 0.25));

		// draw ball
		glm::mat4 M3(base_M);
		M3 = glm::translate(M3, glm::vec3(-1, -1, 0));
		M3 = glm::translate(M3, glm::vec3(displace, displace / 1.2f, 0));
		M3 = glm::rotate(M3, glm::radians(angle), glm::vec3(0, 1, 0));
		M3 = glm::scale(M3, glm::vec3(0.1, 0.1, 0.1));

		if (collapsed == false && time_ >=200) {
			DisplaySphere(M3);
		}
		GLfloat program;
		if (collapsed == false) {
			program = Modelprogram;
		}
		else {
			program = Explosionprogram;
		}

		glUseProgram(program);
		GLfloat vecID;
		vecID = glGetUniformLocation(program, "push_out");
		//float push_out = sinf(degree) * 0.1f + 0.2f ;
		float push_out = (degree)+1.0f;
		glUniform1f(vecID, push_out);

		DrawModel(Eevee, program, Eevee_VAO, Eevee_texture_ID, M2, GL_TRIANGLES);

	}
	else
	{
		// simply draw two pokemons
		glm::mat4 M1(base_M);
		M1 = glm::translate(M1, glm::vec3(-5, -5, 0));
		M1 = glm::rotate(M1, glm::radians(180.0f), glm::vec3(0, 1, 0));

		DrawModel(Umbreon, Modelprogram, Umbreon_VAO, Umbreon_texture_ID, M1, GL_QUADS);

		M2 = glm::translate(M2, glm::vec3(2, -5, 0));
		M2 = glm::scale(M2, glm::vec3(0.25, 0.25, 0.25));

		DrawModel(Eevee, Modelprogram, Eevee_VAO, Eevee_texture_ID, M2, GL_TRIANGLES);

	}
	if (explo_f) {
		time_ += 1;
		if (time_ >= 200) {
			angle += 2;
			degree += 0.25;
			displace += 0.05;
		}
	}
}

void DrawSphere(float radius, float slice, float stack) {
	float theta, phi, xy_step = 360 / slice, z_step = 180 / stack;
	Vertex vert;
	float u, v;
	for (phi = -90; phi <= 90; phi += z_step) {
		VertexAttribute temp;
		for (theta = 0; theta <= 360; theta += xy_step) {
			vert.x = radius * sin(theta * M_PI / 180) * cos(phi * M_PI / 180);
			vert.y = radius * cos(theta * M_PI / 180) * cos(phi * M_PI / 180);
			vert.z = radius * sin(phi * M_PI / 180);
			temp.setPosition(vert);

			u = theta / 360.0f;
			v = (phi + 90.0f) / 180.0f;
			temp.setTexcoord(u, v);

			ball.push_back(temp);

			vert.x = radius * sin(theta * M_PI / 180) * cos((phi + z_step) * M_PI / 180);
			vert.y = radius * cos(theta * M_PI / 180) * cos((phi + z_step) * M_PI / 180);
			vert.z = radius * sin((phi + z_step) * M_PI / 180);
			temp.setPosition(vert);

			u = theta / 360.0f;
			v = (phi + 90.0f + z_step) / 180.0f;
			temp.setTexcoord(u, v);

			ball.push_back(temp);
		}
	}
}

void DisplaySphere(glm::mat4& M) {
	GLfloat pmtx[16];
	GLfloat mmtx[16];

	GLuint program = Fireprogram;

	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");

	glUseProgram(program);

	glPushMatrix();
		//glRotatef(270, 1, 0, 0); // turn to face the user
		glBindTexture(GL_TEXTURE_2D, BallTexture);
		// get current modelview & projection matrix & pass to vertex shader
		glm::mat4 V = getV();
		glm::mat4 P = getP();
		glm::mat4 MV = V * M;

		glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, &MV[0][0]);
		glUniform1i(glGetUniformLocation(program, "ball_texture"), Ball_texture_ID);
		glBindVertexArray(Ball_VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(VertexAttribute) * ball.size());
	glPopMatrix();

	glBindVertexArray(0);
	glActiveTexture(0);
	glUseProgram(0);
}

void DrawModel(Object* model, GLuint program, GLuint vao, GLuint texture_ID, glm::mat4& M, GLenum DrawingMode)
{
	glUseProgram(program);

	GLuint ModelMatrixID = glGetUniformLocation(program, "M");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &M[0][0]);

	glm::mat4 V = getV();
	ModelMatrixID = glGetUniformLocation(program, "V");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &V[0][0]);

	glm::mat4 P = getP();
	ModelMatrixID = glGetUniformLocation(program, "P");
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &P[0][0]);

	glUniform1i(glGetUniformLocation(program, "texture"), texture_ID);

	glBindVertexArray(vao);
	glDrawArrays(DrawingMode, 0, model->positions.size() / 3);
	glBindVertexArray(0);
	glActiveTexture(0);
	glUseProgram(0);

}
