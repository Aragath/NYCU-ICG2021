#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <iostream>
#include <string>

using namespace std;

int windowSize[2] = { 600, 600 };
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);

GLuint program;
GLuint vboName;

//Initialize the shaders 
void shaderInit();
void bindBuffer();

//Number of vertices that will be sent to shader
int verticeNumber = 3;
int verticeSize = 3;

//Collecting vertex datas
float *drawTriangle();
void setPosition(float* vertexArr, float x, float y, float z);

int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");

	glewInit();
	shaderInit();
	bindBuffer();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, frag);
}

void bindBuffer() {
	// Just a example shows how to buffer data.
	// You need to use VAO + VBO in main.cpp to buffer 
	
	// Generate a new buffer object
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	//Copy vertex data to the buffer object
	float* vertices;
	vertices = drawTriangle();
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticeNumber * verticeSize, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	// here using the sample array so the last parameter is 0 which means the offset of the fist component in buffer.
	// It can let Vertex Attribute pointer know where to start load data in buffer
	// you can use the function : "offsetof(ClassName, ComponentName)" to set up last parameter when using the class or stuct.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(0));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void display() {   
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	// get current modelview & projection matrix used in OpenGL rendering
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);

	// get the "location" (It's similar to pointer) of uniform variable in shader
 	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");

	// using shader
	glUseProgram(program);

	// pass the projection matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	// pass the modelview matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	// draw the vertex stored in buffer 
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
	glUseProgram(0);
	glPopMatrix();
	glutSwapBuffers();
}

// generate the vertex infomation of model (here is a simple triangle)
// you may need to contain more infomation (such like : texture coordinate).
// using VertexAttribute class to store correlated data defined in Vertex.h.
float *drawTriangle() {
	float*vertices;
	vertices = new float[verticeNumber * verticeSize];
	setPosition(vertices, 0, 0, 0);
	setPosition(vertices + verticeSize, 1, 0, 0);
	setPosition(vertices + 2 * verticeSize, 0, 1, 0);
	return vertices;
}

void setPosition(float * vertexArr,float x,float y,float z) {
	*vertexArr = x;
	*(vertexArr + 1) = y;
	*(vertexArr + 2) = z;
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {

}

void idle() {
	glutPostRedisplay();
}