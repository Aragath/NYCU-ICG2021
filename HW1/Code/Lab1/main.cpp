#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Object.h"
#include "glut.h"
#include <math.h>
using namespace std;

class Vertex {
	public:
		float x;
		float y;
		float z;
		void normalize() {
			float magnitude;
			magnitude = sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
			*this = *this / magnitude;
		}
		Vertex operator + (Vertex const& obj) {
			Vertex temp;
			temp.x = this->x + obj.x;
			temp.y = this->y + obj.y;
			temp.z = this->z + obj.z;
			return temp;
		}
		Vertex operator - (Vertex const& obj) {
			Vertex temp;
			temp.x = this->x - obj.x;
			temp.y = this->y - obj.y;
			temp.z = this->z - obj.z;
			return temp;
		}
		Vertex operator / (float const& num) {
			Vertex temp;
			temp.x = this->x / num;
			temp.y = this->y / num;
			temp.z = this->z / num;
			return temp;
		}
		friend ostream& operator<<(ostream& os, const Vertex& v) {
			os << v.x << "," << v.y << "," << v.z;
			return os;
		}
};

int windowSize[2];
unsigned int texture;
int width, height, nrChannels;
unsigned char* pikachuTexture;
Object* Clock = new Object("clock.obj");
Object* Pikachu = new Object("Pikachu.obj");

void light();
void display();
void idle();
void reshape(GLsizei, GLsizei);
void InitTexture();
void LoadModel(Object*, bool is_Pikachu = false);
void keyboard(unsigned char key, int x, int y);
void DrawBase();
void DrawClockHand();
void drawUpperBaseTriangles();
void drawLowerBaseTriangles();
void drawRectangles();

// default light color, can be use to reset the material color
GLfloat default_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f};
GLfloat default_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f};

int frame_count = 0;

float hour_hand_angle = 0.0f;
float minute_hand_angle = 0.0f;
float clock_angle = 0.0f;
float pikachu_rot_angle = 0.0f;
float pikachu_rev_angle = 0.0f;

bool flag_r = false;
bool flag_p = false;
bool flag_m = false;


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("HW1");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	InitTexture();
	glutMainLoop();
	return 0;
}

void light()
{
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_position[] = { 10.0, 10.0, 5.0, 0.0 };
	glShadeModel(GL_SMOOTH);
	// z buffer enable
	glEnable(GL_DEPTH_TEST);
	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_specular);
	GLfloat mat_shininess[] = { 100 };
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], 1.0, 1000.0);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 15.0, 40.0,// eye
		0.0, 0.0, 0.0,     // center
		0.0, 1.0, 0.0);    // up

	light();

	////TO DO : add model, translate, rotate, scale ,material

	// add model, translate, rotate, scale
	glPushMatrix();
	glRotatef(pikachu_rev_angle, 0.0f, 1.0f, 0.0f); // revolve y axis
	glTranslatef(14, 0, 0);
	glScalef(10, 10, 10);
	glRotatef(45, 0, 1, 0);
	glRotatef(pikachu_rot_angle, 0.0f, 1.0f, 0.0f); // rotate itself
	LoadModel(Pikachu, true);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0, 7.5, 0);
	glScalef(0.08, 0.08, 0.08);
	glRotatef(clock_angle, 0.0f, 1.0f, 0.0f);
	LoadModel(Clock, false);
	glPopMatrix();


	//
	DrawBase();
	DrawClockHand();

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void idle() {
	if (flag_p) {
		pikachu_rot_angle += 1;
		if (pikachu_rot_angle > 360.0) pikachu_rot_angle -= 360.0;
	}
	if (flag_r) {
		clock_angle += 1;
		if (clock_angle > 360.0) clock_angle -= 360.0;
	}
	if (flag_m) {
		pikachu_rev_angle += 2;
		if (pikachu_rev_angle > 360.0) pikachu_rev_angle -= 360.0;
	}
	// minute hand rotation
	minute_hand_angle += 1;

	// hour hand rotation
	if (frame_count % 12 == 0 && frame_count != 0){
		hour_hand_angle += 1;
	}

	frame_count += 1;
	glutPostRedisplay();
}

void InitTexture() {
	stbi_set_flip_vertically_on_load(true);
	pikachuTexture = stbi_load("Pikachu.png", &width, &height, &nrChannels, 0);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pikachuTexture);

	glDisable(GL_TEXTURE_2D);
}

void LoadModel(Object* Model,bool is_Pikachu) {
	if (is_Pikachu) {
		GLfloat pikachu_diffuse[] = { 1, 1, 0, 1 };
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pikachu_diffuse);
		glEnable(GL_TEXTURE_2D);
	}
	for (size_t i = 0; i < Model->fNum; ++i)
	{
		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j < 3; ++j)
		{
			if (is_Pikachu) {
				int idx = i * 3 * 2 + j * 2;
				glTexCoord2f(Model->texcoords[idx],Model->texcoords[idx+1]);
			}
			glNormal3fv(Model->nList[Model->faceList[i][j].nIndex].ptr);
			glVertex3fv(Model->vList[Model->faceList[i][j].vIndex].ptr);
		}
		glEnd();
	}
	if (is_Pikachu) {
		// set amibent back to default
		glMaterialfv(GL_FRONT, GL_AMBIENT, default_ambient);
		glDisable(GL_TEXTURE_2D);
	}
}

void keyboard(unsigned char key, int x, int y) {
	//// TO DO : implement the keyboard function
	switch (key) {
	case 'r':
		flag_r = !flag_r;
		break;
	case 'p':
		flag_p = !flag_p;
		break;
	case 'm':
		flag_m = !flag_m;
		break;
	}
	
}

void DrawBase() {
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, default_diffuse);

	glRotatef(clock_angle, 0.0f, 1.0f, 0.0f);
	//// TO DO : Draw base 
	glBegin(GL_POLYGON);


	GLfloat blue_diffuse[] = { 114.0f / 255.0f , 201.0f / 255.0f , 240.0f / 255.0f , 1.0f };

	// draw upper base
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-5.0f, 0.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-10.0f, 0.0f, 0.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-5.0f, 0.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 5 * sqrt(3));
	glPopMatrix();

	// draw lower base
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(10.0f, -5.0f, 0.0f);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(5.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-5.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-10.0f, -5.0f, 0.0f);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-5.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(5.0f, -5.0f, 5 * sqrt(3));
	glPopMatrix();

	glEnd();


	//// TO DO : draw the star polygon
	glBegin(GL_TRIANGLES);

	// Draw the triangle with color #f72585
	GLfloat red_diffuse[] = { 247.0f / 255.0f , 37.0f / 255.0f , 133.0f / 255.0f , 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);

	// draw upper base triangles
	glPushMatrix();
	drawUpperBaseTriangles();
	glPopMatrix();
	
	// draw lower base triangles
	glPushMatrix();
	drawLowerBaseTriangles();
	glPopMatrix();
	
	glEnd();

	//// TO DO : draw the rectangle edge
	glBegin(GL_QUADS);

	// draw rectangles
	glPushMatrix();
	drawRectangles();
	glPopMatrix();

	glEnd();
	}

void DrawClockHand(){
	// draw hour hand
	glPushMatrix();
	GLUquadricObj* hour_hand = gluNewQuadric();
	GLfloat hour_hand_diffuse[] = { 0.0f , 1.0f , 1.0f , 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, hour_hand_diffuse);
	glTranslatef(0, 7.5, 0);
	
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(hour_hand_angle, 0.0f, 1.0f, 0.0f);		// normal hour hand behavior
	glRotatef(clock_angle, 0.0f, 1.0f, 0.0f);			// press 'r'
	gluCylinder(hour_hand, 0.3, 0.3, 3, 30, 30);		
	glPopMatrix();
	
	// draw minute hand
	glPushMatrix();
	GLUquadricObj* minute_hand = gluNewQuadric();
	GLfloat minute_hand_diffuse[] = { 1.0f , 0.0f , 1.0f , 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, minute_hand_diffuse);
	glTranslatef(0, 7.5, 0);
	glRotatef(90.0f, 0.0f, 0.0f, -1.0f);				// point the hand to 15 when started
	glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(minute_hand_angle, 0.0f, 1.0f, 0.0f);		// normal minute hand behavior
	glRotatef(clock_angle, 0.0f, 1.0f, 0.0f);			// press 'r'
	gluCylinder(minute_hand, 0.3, 0.3, 5, 30, 30);
	glPopMatrix();
}

void drawUpperBaseTriangles()
{
	GLfloat red_diffuse[] = { 247.0f / 255.0f , 37.0f / 255.0f , 133.0f / 255.0f , 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(15.0f, 0.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, -5 * sqrt(3));

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, -10 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-5.0f, 0.0f, -5 * sqrt(3));

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-5.0f, 0.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-15.0f, 0.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-10.0f, 0.0f, 0.0f);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-10.0f, 0.0f, 0.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-15.0f, 0.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-5.0f, 0.0f, 5 * sqrt(3));

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-5.0f, 0.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 10 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 5 * sqrt(3));

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(5.0f, 0.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(15.0f, 0.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(10.0f, 0.0f, 0.0f);
}

void drawLowerBaseTriangles()
{
	GLfloat red_diffuse[] = { 247.0f / 255.0f , 37.0f / 255.0f , 133.0f / 255.0f , 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(10.0f, -5.0f, 0.0f);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(15.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(5.0f, -5.0f, -5 * sqrt(3));

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(5.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, -5.0f, -10 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-5.0f, -5.0f, -5 * sqrt(3));

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-5.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-15.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-10.0f, -5.0f, 0.0f);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-10.0f, -5.0f, 0.0f);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-15.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-5.0f, -5.0f, 5 * sqrt(3));

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-5.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, -5.0f, 10 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(5.0f, -5.0f, 5 * sqrt(3));

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(5.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(15.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(10.0f, -5.0f, 0.0f);
}

void drawRectangles()
{
	// Draw the rectangle with color #f72585
	GLfloat red_diffuse[] = { 247.0f / 255.0f , 37.0f / 255.0f , 133.0f / 255.0f , 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);

	glNormal3f(0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(10.0f, 0.0f, 0.0f);
	glNormal3f(0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(10.0f, -5.0f, 0.0f);
	glNormal3f(0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(15.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(15.0f, 0.0f, -5 * sqrt(3));

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(15.0f, 0.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(15.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(5.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(5.0f, 0.0f, -5 * sqrt(3));

	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(5.0f, 0.0f, -5 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(5.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(0.0f, -5.0f, -10 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(0.0f, 0.0f, -10 * sqrt(3));

	glNormal3f(-0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(0.0f, 0.0f, -10 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(0.0f, -5.0f, -10 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(-5.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(-5.0f, 0.0f, -5 * sqrt(3));

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-5.0f, 0.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-5.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-15.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-15.0f, 0.0f, -5 * sqrt(3));

	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(-15.0f, 0.0f, -5 * sqrt(3));
	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(-15.0f, -5.0f, -5 * sqrt(3));
	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(-10.0f, -5.0f, 0.0f);
	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(-10.0f, 0.0f, 0.0f);

	glNormal3f(-0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(-10.0f, 0.0f, 0.0f);
	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(-10.0f, -5.0f, 0.0f);
	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(-15.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(-15.0f, 0.0f, 5 * sqrt(3));

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-15.0f, 0.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-15.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-5.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-5.0f, 0.0f, 5 * sqrt(3));

	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(-5.0f, 0.0f, 5 * sqrt(3));
	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(-5.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(0.0f, -5.0f, 10 * sqrt(3));
	glNormal3f(-0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(0.0f, 0.0f, 10 * sqrt(3));

	glNormal3f(0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(0.0f, 0.0f, 10 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(0.0f, -5.0f, 10 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(5.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, 0.5f);
	glVertex3f(5.0f, 0.0f, 5 * sqrt(3));

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(5.0f, 0.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(5.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(15.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(15.0f, 0.0f, 5 * sqrt(3));

	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(15.0f, 0.0f, 5 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(15.0f, -5.0f, 5 * sqrt(3));
	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(10.0f, -5.0f, 0.0f);
	glNormal3f(0.5 * sqrt(3), 0.0f, -0.5f);
	glVertex3f(10.0f, 0.0f, 0.0f);

}