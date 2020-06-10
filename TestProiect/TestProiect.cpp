#pragma region header

#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <gl/glut.h>
#include <math.h>
#include <SOIL.h>
#include <imageloader.h>

#pragma endregion

#pragma region def
#define PI 3.1415927
#define BASE_HEIGHT 0.25
#define BASE_WIDE  0.05
#define	imageWidth 64
#define	imageHeight 64

#pragma endregion

#pragma region globalVariables
GLfloat* tex;
const int TEXDIM = 1024;

GLuint startList;
GLUquadricObj* qobj, * sphere;
GLfloat lightpos[4] = { 0, 22, -420, 1.f };
GLfloat lightpos_ext[4] = { -90, 120, -600, 1.f };

GLfloat rightwallshadow[4][4];
GLfloat floorshadow[4][4];
GLfloat floorshadow_ext[4][4];
GLfloat leftwallshadow[4][4];
GLfloat tableshadow[4][4];

#pragma endregion

#pragma region enums

static GLint fogMode;

enum THEME {
	INTERIOR_T1, INTERIOR_T2, EXTERIOR_T1, EXTERIOR_T2, EXTERIOR_T3
};

enum {
	X, Y, Z, W
};

enum {
	A, B, C, D
};

enum {
	NONE, SHADOW
};

int CURRENT_THEME = INTERIOR_T1;

#pragma endregion

void init() {

	startList = glGenLists(2);
	qobj = gluNewQuadric();
	sphere = gluNewQuadric();

	gluQuadricDrawStyle(qobj, GLU_FILL); // flat shaded 
	gluQuadricNormals(qobj, GLU_FLAT);
	glNewList(startList, GL_COMPILE);
	gluCylinder(qobj, 20, 7, 10.0, 10, 20);
	glEndList();

	glNewList(startList + 1, GL_COMPILE);
	gluSphere(sphere, 5.f, 20, 20);
	gluDeleteQuadric(sphere);
	glEndList();

}

void Load_Texture1(void)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height;
	unsigned char* image = SOIL_load_image("flori.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

}

void Load_Texture2(void)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height;
	unsigned char* image = SOIL_load_image("curte2.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

}

GLfloat* make_texture(int maxs, int maxt)
{
	int s, t;
	static GLfloat* texture;

	texture = (GLfloat*)malloc(maxs * maxt * sizeof(GLfloat));
	for (t = 0; t < maxt; t++) {
		for (s = 0; s < maxs; s++) {
			float startText = 0.9;
			float val = (t - 512) * (t - 512) + (s - 512) * (s - 512);
			float startRadiusSqr = 262144;
			float radiusSqr = startRadiusSqr;
			texture[s + maxs * t] = 1;
			while (radiusSqr > 0 && val < radiusSqr)
			{
				texture[s + maxs * t] = startText;
				if (radiusSqr >= startRadiusSqr / 2)
				{
					startText -= 0.1;
				}
				else
				{
					startText += 0.1;
				}
				radiusSqr -= startRadiusSqr / 10;
			}
		}
	}
	return texture;
}

GLfloat* make_texture2(int maxs, int maxt)
{

	int s, t;
	static GLfloat* texture;

	texture = (GLfloat*)malloc(maxs * maxt * sizeof(GLfloat));
	for (t = 0; t < maxt; t++) {
		for (s = 0; s < maxs; s++) {
			if (t % 2 > 0)
				texture[s + maxs * t] = 1;
		}
	}
	return texture;
}

GLfloat* make_texture3(int maxs, int maxt)
{
	int s, t;
	static GLfloat* texture;

	texture = (GLfloat*)malloc(maxs * maxt * sizeof(GLfloat));
	for (t = 0; t < maxt; t++) {
		for (s = 0; s < maxs; s++) {
			texture[s + maxs * t] = ((s >> 4) & 0x1) ^ ((t >> 4) & 0x1);
		}
	}
	return texture;
}

void draw_bezier_curve() {

	GLfloat ctrlpoints[4][3] = {
		{ 0, 30.0, 0 }, { 10, 25, -0 },
		{-10.0, 15, 0 }, {0, 10, -0 } };
	GLfloat colctrlpoints[4][4] = {
		{ 0.3, 1.0, 0.5, 1.0}, { 0.2, 0.1, 0.5, 1.0},
		{0.6, 1, 0.5, 1.0}, {0.7, 0.1, 0.4, 1.0} };


	glShadeModel(GL_FLAT);
	glMap1f(GL_MAP1_VERTEX_3, 0.0, 1, 3, 4, &ctrlpoints[0][0]);
	glMap1f(GL_MAP1_COLOR_4, 0.0, 1, 4, 4, &colctrlpoints[0][0]);
	glEnable(GL_MAP1_VERTEX_3);
	glEnable(GL_MAP1_COLOR_4);

	glLineWidth(4.0);
	glBegin(GL_LINE_STRIP);
	for (int j = 0; j <= 50; j++)
		glEvalCoord1f((GLfloat)j / 50.0);
	glEnd();

	glDisable(GL_MAP1_VERTEX_3);
	glDisable(GL_MAP1_COLOR_4);
}

void shadowmatrix(GLfloat shadowMat[4][4],
	GLfloat groundplane[4],
	GLfloat lightpos[4])
{
	GLfloat dot;

	// find dot product between light position vector and ground plane normal 
	dot = groundplane[X] * lightpos[X] +
		groundplane[Y] * lightpos[Y] +
		groundplane[Z] * lightpos[Z] +
		groundplane[W] * lightpos[W];

	shadowMat[0][0] = dot - lightpos[X] * groundplane[X];
	shadowMat[1][0] = 0.f - lightpos[X] * groundplane[Y];
	shadowMat[2][0] = 0.f - lightpos[X] * groundplane[Z];
	shadowMat[3][0] = 0.f - lightpos[X] * groundplane[W];

	shadowMat[X][1] = 0.f - lightpos[Y] * groundplane[X];
	shadowMat[1][1] = dot - lightpos[Y] * groundplane[Y];
	shadowMat[2][1] = 0.f - lightpos[Y] * groundplane[Z];
	shadowMat[3][1] = 0.f - lightpos[Y] * groundplane[W];

	shadowMat[X][2] = 0.f - lightpos[Z] * groundplane[X];
	shadowMat[1][2] = 0.f - lightpos[Z] * groundplane[Y];
	shadowMat[2][2] = dot - lightpos[Z] * groundplane[Z];
	shadowMat[3][2] = 0.f - lightpos[Z] * groundplane[W];

	shadowMat[X][3] = 0.f - lightpos[W] * groundplane[X];
	shadowMat[1][3] = 0.f - lightpos[W] * groundplane[Y];
	shadowMat[2][3] = 0.f - lightpos[W] * groundplane[Z];
	shadowMat[3][3] = dot - lightpos[W] * groundplane[W];

}

void findplane(GLfloat plane[4],
	GLfloat v0[3], GLfloat v1[3], GLfloat v2[3])
{
	GLfloat vec0[3], vec1[3];

	// need 2 vectors to find cross product 
	vec0[X] = v1[X] - v0[X];
	vec0[Y] = v1[Y] - v0[Y];
	vec0[Z] = v1[Z] - v0[Z];

	vec1[X] = v2[X] - v0[X];
	vec1[Y] = v2[Y] - v0[Y];
	vec1[Z] = v2[Z] - v0[Z];

	// find cross product to get A, B, and C of plane equation 
	plane[A] = vec0[Y] * vec1[Z] - vec0[Z] * vec1[Y];
	plane[B] = -(vec0[X] * vec1[Z] - vec0[Z] * vec1[X]);
	plane[C] = vec0[X] * vec1[Y] - vec0[Y] * vec1[X];

	plane[D] = -(plane[A] * v0[X] + plane[B] * v0[Y] + plane[C] * v0[Z]);
}
void draw_leg(float xt, float yt, float zt, float sx, float sy, float sz, float dim)
{
	glPushMatrix();
	glTranslatef(xt, yt, zt);
	glScalef(sx, sy, sz);
	glutSolidCube(dim);
	glPopMatrix();
}

void draw_table()
{
	static GLfloat table_legs_mat[] = { 1.0, 0.65, 0.3, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, table_legs_mat);
	draw_leg(90, -100, -450, 0.1, 0.5, 0.1, 50);
	draw_leg(40, -100, -450, 0.1, 0.5, 0.1, 50);
	draw_leg(40, -100, -480, 0.1, 0.5, 0.1, 50);
	draw_leg(90, -100, -480, 0.1, 0.5, 0.1, 50);

	static GLfloat table_mat[] = { 1.0, 0.65, 0.3, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, table_mat);
	glPushMatrix();
	glTranslatef(65, -87.5, -475);
	glScalef(1, 0.01, 1);
	glutSolidCube(75);
	glPopMatrix();

}

void draw_lamp()
{

	glShadeModel(GL_FLAT);
	glPushMatrix();
	glTranslatef(0, 65, -420);

	static GLfloat lamp_leg_mat[] = { 0.2, 0.2, 0.2, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lamp_leg_mat);

	static GLfloat bc_wall_mat[] = { 0.7, 0.24, 0.6, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, bc_wall_mat);
	draw_bezier_curve();

	static GLfloat lamp_mat[] = { 0.92, 0.76, 0.9, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lamp_mat);

	glPushMatrix();
	glRotatef(270.0, 1.0, 0.0, 0.0);
	glCallList(startList);

	glDisable(GL_LIGHTING);
	glColor3f(1.f, 1.f, .7f);
	glCallList(startList + 1);
	glEnable(GL_LIGHTING);

	glPopMatrix();

	glPopMatrix();

}

void draw_window() {

	// -------------- draw  white border ---------------- //

	glBegin(GL_QUADS);
	static GLfloat window_border[] = { 1, 1, 1, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, window_border);
	glNormal3f(0.f, 0.f, 1.f);
	glVertex3f(-32.f, -42.f, -519.f);
	glVertex3f(32.f, -42.f, -519.f);
	glVertex3f(32.f, 42.f, -519.f);
	glVertex3f(-32.f, 42.f, -519.f);
	glEnd();


	glEnable(GL_TEXTURE_2D);
	Load_Texture2();

	// ------------- draw blue window -------------- //
	glBegin(GL_QUADS);
	static GLfloat window[] = { 1, 1, 1, 0.4 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, window);
	//glNormal3f(0.f, 0.f, 1.f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-30.f, -40.f, -518);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(30.f, -40.f, -518.f);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(30.f, 40.f, -518.f);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-30.f, 40.f, -518.f);
	glDisable(GL_TEXTURE_2D);
	glEnd();


	// -------------- draw white line for window  ---------------- //
	static GLfloat window_line[] = { 0.8, 0.8, 0.8, 0.4 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, window_line);
	glBegin(GL_QUADS);
	glNormal3f(0.f, 0.f, 1.f);
	glVertex3f(-30, -1.f, -517.f);
	glVertex3f(30, -1.f, -517.f);
	glVertex3f(30, 1.f, -517.f);
	glVertex3f(-30, 1.f, -517.f);
	glEnd();

	// -------------- draw white line for window  ---------------- //
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, window_line);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(-1, -40, -517);
	glVertex3f(1, -40, -517);
	glVertex3f(1, 40, -517);
	glVertex3f(-1, 40, -517);
	glEnd();


}

void draw_teapot()
{
	static GLfloat teapot[] = { 0.65, 0.65, 0.87, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, teapot);
	glutSolidTeapot(1.0);
}

void draw_picture() {


	glPushMatrix();
	glTranslatef(-65, 20, 40);

	Load_Texture1();

	// --------- picture --------- // 

	static GLfloat picture[] = { 0, 0.5, 0, 0.4 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, picture);

	glBegin(GL_QUADS);

	// Fata de jos
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-30.f, -20.f, -320.f);
	glVertex3f(-30.f, -20.f, -520.f);
	glVertex3f(-25.f, 20.f, -520.f);
	glVertex3f(-25.f, -20.f, -320.f);

	// Fata de sus
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-25.f, 20.f, -320.f);
	glVertex3f(-25.f, 20.f, -520.f);
	glVertex3f(-30.f, 20.f, -520.f);
	glVertex3f(-30.f, 20.f, -320.f);

	// Lateral dreapta
	glNormal3f(0.f, 1.f, 0.f);
	glVertex3f(-25.f, -20.f, -520.f);
	glVertex3f(-30.f, -20.f, -520.f);
	glVertex3f(-30.f, 20.f, -520.f);
	glVertex3f(-25.f, 20.f, -520.f);

	// Lateral stanga
	glNormal3f(0.f, -1.f, 0.f);
	glVertex3f(-30.f, -20.f, -320.f);
	glVertex3f(-25.f, -20.f, -320.f);
	glVertex3f(-25.f, 20.f, -320.f);
	glVertex3f(-30.f, 20.f, -320.f);

	// Fata spate
	glNormal3f(-1.f, 0.f, 0.f);
	glVertex3f(-30.f, -20.f, -520.f);
	glVertex3f(-30.f, -20.f, -320.f);
	glVertex3f(-30.f, 20.f, -320.f);
	glVertex3f(-30.f, 20.f, -520.f);

	static GLfloat picture2[] = { 1, 1, 1, 0.4 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, picture2);

	// Frontal
	glNormal3f(1.f, 0.f, 0.f);
	glEnable(GL_TEXTURE_2D);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-25.f, -20.f, -320.f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-25.f, -20.f, -520.f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-25.f, 20.f, -520.f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-25.f, 20.f, -320.f);
	glDisable(GL_TEXTURE_2D);
	glEnd();

	glPopMatrix();


}

void draw_torus() {
	glutSolidTorus(5, 10, 5, 20);
}

void draw_icosahedron(float x, float y, float z, float scaleIco, float scaleStickXZ, float scaleStickY, float dim) {
	glPushMatrix();
	glTranslatef(x, y, z);
	static GLfloat ico_leg[] = { 1, 1, 1, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ico_leg);
	draw_leg(0, -30, -0, scaleStickXZ, scaleStickY, scaleStickXZ, dim);
	glScalef(scaleIco, scaleIco, scaleIco);
	static GLfloat ico[] = { 0.6f, 0.8f, 0.4f, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, ico);
	glutSolidIcosahedron();
	glPopMatrix();
}

void draw_icosahedron1() {
	draw_icosahedron(-80, -50, -455, 10, 0.03, 0.5, 100);
}

void draw_icosahedron2() {
	draw_icosahedron(-55, -30, -425, 13, 0.03, 0.8, 100);
}

void draw_icosahedron3() {
	draw_icosahedron(-90, -50, -350, 10, 0.03, 0.5, 100);
}

void draw_table_items() {
	glPushMatrix();
	glRotatef(0.0, 0.0, 0.0, 1.0);
	glTranslatef(50, -80, -465);
	glScalef(8.0, 8.0, 8.0);
	draw_teapot();
	glPopMatrix();
	glFlush();

	glPushMatrix();
	glRotatef(0.0, 0.0, 0.0, 1.0);
	glTranslatef(60, -84, -465);
	glScalef(4.0, 4.0, 4.0);
	draw_teapot();
	glPopMatrix();
	glFlush();

	glPushMatrix();
	glRotatef(0.0, 0.0, 0.0, 1.0);
	glTranslatef(70, -84, -465);
	glScalef(4.0, 4.0, 4.0);
	draw_teapot();
	glPopMatrix();
	glFlush();

	glPushMatrix();
	glTranslatef(85, -84, -459);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	draw_torus();
	glPopMatrix();
}

void redrawInterior() {

	static GLfloat lr_wall_mat[] = { 0.6f, 0.6f, 0.6f, 0 };
	static GLfloat b_wall_mat[] = { 0.76, 0.67, 0.4, 0 };
	static GLfloat c_wall_mat[] = { 0.5, 0.5, 0.5, 0 };
	static GLfloat f_wall_mat[] = { 0.9, 0.55, 0.9, 0 };

	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, f_wall_mat);


	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	GLfloat white[] = { 0.5, 0.5, 0.5, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);

	glDisable(GL_BLEND);
	glDisable(GL_FOG);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);

	switch (CURRENT_THEME)
	{
	case INTERIOR_T1:
		tex = make_texture(TEXDIM, TEXDIM);
		break;
	default:
		tex = make_texture2(TEXDIM, TEXDIM);
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, 1, TEXDIM, TEXDIM, 0, GL_RED, GL_FLOAT, tex);
	free(tex);
	glEnable(GL_TEXTURE_2D);


	// ------- floor --------- //

	glBegin(GL_QUADS);
	glNormal3f(0.f, 1.f, 0.f);
	glTexCoord2i(0, 0);
	glVertex3f(-100.f, -100.f, -320.f);
	glTexCoord2i(1, 0);
	glVertex3f(100.f, -100.f, -320.f);
	glTexCoord2i(1, 1);
	glVertex3f(100.f, -100.f, -520.f);
	glTexCoord2i(0, 1);
	glVertex3f(-100.f, -100.f, -520.f);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor4f(0.2f, 0.2f, 0.2f, 0.5f);  // shadow color 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glMultMatrixf((GLfloat*)floorshadow);
	draw_table();
	glPopMatrix();

	glPushMatrix();
	glMultMatrixf((GLfloat*)floorshadow);
	draw_icosahedron1();
	draw_icosahedron2();
	draw_icosahedron3();
	glPopMatrix();
	glDisable(GL_BLEND);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);




	// --------- left wall --------- // 

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lr_wall_mat);
	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);
	glNormal3f(1.f, 0.f, 0.f);
	glVertex3f(-100.f, -100.f, -320.f);
	glVertex3f(-100.f, -100.f, -520.f);
	glVertex3f(-100.f, 100.f, -520.f);
	glVertex3f(-100.f, 100.f, -320.f);
	glEnd();


	glStencilFunc(GL_EQUAL, 1, 1);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_LIGHTING);
	glColor4f(0.2f, 0.2f, 0.2f, 0.5f);  // shadow color 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glMultMatrixf((GLfloat*)leftwallshadow);
	draw_icosahedron1();
	draw_icosahedron2();
	draw_icosahedron3();
	glPopMatrix();
	glDisable(GL_BLEND);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);



	// --------- right wall --------- // 

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, lr_wall_mat);
	glBegin(GL_QUADS);
	glNormal3f(-1.f, 0.f, 0.f);
	glVertex3f(100.f, -100.f, -320.f);
	glVertex3f(100.f, 100.f, -320.f);
	glVertex3f(100.f, 100.f, -520.f);
	glVertex3f(100.f, -100.f, -520.f);
	glEnd();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor4f(0.22f, 0.22f, 0.22f, 0.5f);  // shadow color 
	glPushMatrix();
	glMultMatrixf((GLfloat*)rightwallshadow);
	draw_table();
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);



	// --------- ceiling --------- // 

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, c_wall_mat);
	glBegin(GL_QUADS);
	glNormal3f(0.f, -1.f, 0.f);
	glVertex3f(-100.f, 100.f, -320.f);
	glVertex3f(-100.f, 100.f, -520.f);
	glVertex3f(100.f, 100.f, -520.f);
	glVertex3f(100.f, 100.f, -320.f);
	glEnd();



	// --------- back wall --------- // 

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, b_wall_mat);
	glBegin(GL_QUADS);
	glNormal3f(0.f, 0.f, 1.f);
	glVertex3f(-100.f, -100.f, -520.f);
	glVertex3f(100.f, -100.f, -520.f);
	glVertex3f(100.f, 100.f, -520.f);
	glVertex3f(-100.f, 100.f, -520.f);
	glEnd();



	// ---------  draw table ---------- /

	draw_table();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor4f(0.2f, 0.2f, 0.2f, 0.5f);  // shadow color 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glMultMatrixf((GLfloat*)tableshadow);
	draw_table_items();
	glPopMatrix();
	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);


	draw_table_items();
	draw_icosahedron1();
	draw_icosahedron2();
	draw_icosahedron3();
	draw_lamp();
	draw_window();
	draw_picture();

	// ----------------- reset color ------------------- //

	//glColor3f(1.0, 1.0, 1.0);
	glutSwapBuffers();    // high end machines may need this 
}

void draw_torus1()
{
	glPushMatrix();
	glTranslatef(80, -95, -420);
	glScalef(2.0, 2.0, 2.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	static GLfloat torsus_mat[] = { 0.45, 0.85, 0.47, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, torsus_mat);
	glutSolidTorus(5, 10, 5, 20);
	glPopMatrix();
	glFlush();
}

void draw_torus2()
{
	glPushMatrix();
	glTranslatef(80, -80, -420);
	glScalef(1.5, 1.5, 1.5);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	static GLfloat torsus_mat[] = { 0.45, 0.35, 0.87, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, torsus_mat);
	glutSolidTorus(5, 10, 5, 20);
	glPopMatrix();
	glFlush();
}

void draw_torus3()
{
	glPushMatrix();
	glTranslatef(80, -70, -420);
	glScalef(1.0, 1.0, 1.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	static GLfloat torsus_mat[] = { 0.25, 0.65, 0.57, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, torsus_mat);
	glutSolidTorus(5, 10, 5, 20);
	glPopMatrix();
	glFlush();
}

void draw_torus4()
{
	glPushMatrix();
	glTranslatef(80, -63, -420);
	glScalef(0.5, 0.5, 0.5);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	static GLfloat torsus_mat[] = { 0.35, 0.55, 0.77, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, torsus_mat);
	glutSolidTorus(5, 10, 5, 20);
	glPopMatrix();
	glFlush();
}

void nor1(void) {

	glutSolidSphere(10, 50, 50);

	glPushMatrix();
	glTranslatef(95, 100, -600);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(6, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(110, 100, -600);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(9, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(130, 100, -600);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(7, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(140, 100, -600);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(4, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(120, 110, -600);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(6, 50, 50);
	glPopMatrix();
}

void nor2(void) {

	glutSolidSphere(10, 50, 50);

	glPushMatrix();
	glTranslatef(25, 140, -550);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(6, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(40, 140, -550);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(9, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(60, 140, -550);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(7, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(70, 140, -550);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(4, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(50, 150, -550);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(6, 50, 50);
	glPopMatrix();
}

void nor3(void) {

	glutSolidSphere(10, 50, 50); // 90, 125, -600

	glPushMatrix();
	glTranslatef(-60, 60, -600);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(6, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-75, 60, -600);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(9, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-95, 60, -600);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(7, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-105, 60, -600);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(4, 50, 50);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-85, 70, -600);
	glScalef(2.0, 2.0, 2.0);
	glutSolidSphere(6, 50, 50);
	glPopMatrix();
}


void crengi() {
	GLUquadricObj* pObj;
	pObj = gluNewQuadric();
	gluQuadricNormals(pObj, GLU_SMOOTH);
	glPushMatrix();
	glTranslatef(0, 27, 0);
	glRotatef(330, 1, 0, 0);
	gluCylinder(pObj, 0.6, 0.1, 15, 25, 25);
	glPopMatrix();
}

void coroana() {
	glPushMatrix();
	glScaled(5, 5, 5);
	glTranslatef(0, 7, 3);
	glutSolidDodecahedron();
	glPopMatrix();
}

void copac() {

	//copac
	static GLfloat tree_mat[] = { 0.5, 0.5, 0.2, 0 };
	static GLfloat coroana_mat[] = { 0.07, 0.46, 0.05, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tree_mat);
	glPushMatrix();
	glTranslatef(-80, -100, -455);
	glScalef(1.2, 1.2, 1.2);
	glRotatef(90, 0, 1, 0);
	GLUquadricObj* pObj;
	pObj = gluNewQuadric();
	gluQuadricNormals(pObj, GLU_SMOOTH);

	glPushMatrix();
	glRotatef(270, 1, 0, 0);
	gluCylinder(pObj, 4, 0.7, 30, 25, 25);
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tree_mat);
	crengi();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, coroana_mat);
	coroana();

	glPushMatrix();
	glScalef(1.5, 1.5, 1.5);
	glTranslatef(0, 25, 25);
	glRotatef(250, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tree_mat);
	crengi();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, coroana_mat);
	coroana();
	glPopMatrix();

	glPushMatrix();
	glScalef(1.8, 1.8, 1.8);
	glTranslatef(0, -6, 21.5);
	glRotatef(-55, 1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, tree_mat);
	crengi();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, coroana_mat);
	coroana();
	glPopMatrix();
	glPopMatrix();

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, coroana_mat);
	coroana();
}

void draw_house() {

	glPushMatrix();
	glTranslatef(0, -50, -600);
	glScalef(5.0, 5.0, 5.0);
	static GLfloat house_mat[] = { 0.22, 0.12, 0.45, 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, house_mat);
	glutSolidCube(20);
	glPopMatrix();
}

void draw_roof() {

	glPushMatrix();
	glTranslatef(0, 0, -600);
	glRotatef(-90.0, 1, 0, 0);
	glRotatef(-90.0, 0, 0, 1);
	glScalef(7.0, 7.0, 7.0);
	static GLfloat roof_mat[] = { 0.78, 0.46, 0.26, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, roof_mat);
	glutSolidCone(10, 10, 4, 4);
	glPopMatrix();
}

void draw_stairs() {

	glPushMatrix();
	glTranslatef(0, -95, -530);
	glScalef(6.0, 1.0, 1.0);
	static GLfloat stairs_mat[] = { 0.66, 0.66, 0.66, 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, stairs_mat);
	glutSolidCube(4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -98, -530);
	glScalef(8.0, 2.0, 1.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, stairs_mat);
	glutSolidCube(4);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -101, -530);
	glScalef(10.0, 1.0, 1.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, stairs_mat);
	glutSolidCube(4);
	glPopMatrix();
}

void draw_window_ext() {

	glBegin(GL_QUADS);
	static GLfloat window[] = { 1, 1, 1, 0.4 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, window);
	glVertex3f(-19.f, -13.f, -518);
	glVertex3f(19.f, -13.f, -518.f);
	glVertex3f(19.f, 13.f, -518.f);
	glVertex3f(-19.f, 13.f, -518.f);
	glEnd();

}

void draw_door() {

	glBegin(GL_QUADS);
	static GLfloat window_border[] = { 0, 0, 0, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, window_border);
	glNormal3f(0.f, 0.f, 1.f);

	glVertex3f(-15.f, -20.f, -519.f);
	glVertex3f(15.f, -20.f, -519.f);
	glVertex3f(15.f, 20.f, -519.f);
	glVertex3f(-15.f, 20.f, -519.f);

	glEnd();

	glBegin(GL_QUADS);
	static GLfloat door[] = { 0.32, 0.32, 0.32, 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, door);

	glVertex3f(-13.f, -20.f, -518);
	glVertex3f(13.f, -20.f, -518.f);
	glVertex3f(13.f, 20.f, -518.f);
	glVertex3f(-13.f, 20.f, -518.f);
	glEnd();


}

void draw_handle() {

	static GLfloat door2[] = { 0.0, 0.0, 0.0, 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, door2);
	glutSolidSphere(5, 10, 10);
}

void draw_sun() {

	static GLfloat sun_mat[] = { 1.0, 1.0, 0.0, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sun_mat);
	glPushMatrix();
	glTranslatef(lightpos_ext[0], lightpos_ext[1], lightpos_ext[2]);
	glScalef(2.5, 2.5, 2.5);
	glutSolidSphere(10, 100, 40);
	glPopMatrix();
}

void draw_fog() {


	glEnable(GL_FOG);
	{
		GLfloat fogColor[4] = { 0.5, 0.5, 0.5, 0.3 };
		fogMode = GL_LINEAR;
		glFogi(GL_FOG_MODE, fogMode);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, 0.5);
		glHint(GL_FOG_HINT, GL_DONT_CARE);
		glFogf(GL_FOG_START, 0);
		glFogf(GL_FOG_END, 700);

	}

	//glClearColor(0.5, 0.5, 0.5, 1.0);  // fog color 

}


void redrawExterior() {

	if (CURRENT_THEME == EXTERIOR_T3)
	{
		glClearColor(0.4, 0.4, 0.4, 0);
	}
	else
	{
		glClearColor(0.01, 0.36, 1.0, 1.0);
	}

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	if (CURRENT_THEME == EXTERIOR_T3)
	{
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA_SATURATE, 8687687);
		draw_fog();
	}
	else
	{
		glDisable(GL_FOG);
		glDisable(GL_BLEND);
	}

	//      * LIGHT * 

	GLfloat white[] = { 0.8, 0.8, 0.8, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos_ext);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);



	switch (CURRENT_THEME)
	{
	case EXTERIOR_T1:
		tex = make_texture3(TEXDIM, TEXDIM);
		break;
	default:
		tex = make_texture(TEXDIM, TEXDIM);
		break;
	}


	glTexImage2D(GL_TEXTURE_2D, 0, 1, TEXDIM, TEXDIM, 0, GL_RED, GL_FLOAT, tex);
	free(tex);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// ----------floor  -- make the floor textured -- ---------- // 

	glBegin(GL_QUADS);
	static GLfloat grass_mat[] = { 0.0, 0.8, 0.0, 0.5 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, grass_mat);
	glNormal3f(0.f, 1.f, 0.f);
	glTexCoord2i(0, 0);
	glVertex3f(-1000.f, -100.f, 1000.f);
	glTexCoord2i(1, 0);
	glVertex3f(1000.f, -100.f, 1000.f);
	glTexCoord2i(1, 1);
	glVertex3f(1000.f, -100.f, -1000.f);
	glTexCoord2i(0, 1);
	glVertex3f(-1000.f, -100.f, -1000.f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	if (CURRENT_THEME == EXTERIOR_T3)
	{
		glEnable(GL_BLEND);
	}

	//      * SHADOW *
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor4f(0.2f, 0.2f, 0.2f, 0.5f);  // shadow color

	if (CURRENT_THEME != EXTERIOR_T3)
	{
		glEnable(GL_BLEND);
	}
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA);
	glPushMatrix();
	glMultMatrixf((GLfloat*)floorshadow_ext);
	copac();
	draw_house();
	draw_roof();
	draw_torus1();
	draw_torus2();
	draw_torus3();
	draw_torus4();
	glPopMatrix();
	//glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);
	glBlendFunc(GL_BLEND_SRC, GL_BLEND_DST);

	if (CURRENT_THEME != EXTERIOR_T3)
	{
		glDisable(GL_BLEND);
	}


	//glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	copac();
	draw_torus1();
	draw_torus2();
	draw_torus3();
	draw_torus4();
	draw_house();
	draw_roof();
	draw_sun();
	draw_stairs();


	glPushMatrix();
	glTranslatef(0, -20, -20);
	draw_window_ext();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -70, -20);
	draw_door();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(10.f, -70.f, -539.f);
	glScalef(0.5, 0.5, 0.5);
	draw_handle();
	glPopMatrix();



	// nori
	static GLfloat nori_mat[] = { 0.55, 0.66, 0.66, 0.5 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, nori_mat);
	nor1();
	nor2();
	nor3();


	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y)
{
	float sensibilityLR = 0.01;
	float sensibilityFWD = 0.8;

	float leftRight = 0;
	float fwd = 0;

	switch (key) {
	case 'a':
		leftRight = -sensibilityLR;
		break;
	case 'd':
		leftRight = +sensibilityLR;
		break;
	case 'w':
		fwd = -sensibilityFWD;
		break;
	case 's':
		fwd = +sensibilityFWD;
		break;
	}

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0, 0, fwd, leftRight, 0, -1, 0.0, 1, 0);
	glutPostRedisplay();
}

void key(unsigned char key, int x, int y)
{
	if (key == '\033')
		exit(0);
}


void menu(int selection)
{
	CURRENT_THEME = selection;
	switch (CURRENT_THEME) {
	case INTERIOR_T1:
		glutDisplayFunc(redrawInterior);
		break;
	case INTERIOR_T2:
		glutDisplayFunc(redrawInterior);
		break;
	case EXTERIOR_T1:
		glutDisplayFunc(redrawExterior);
		break;
	case EXTERIOR_T2:
		glutDisplayFunc(redrawExterior);
		break;
	case EXTERIOR_T3:
		glutDisplayFunc(redrawExterior);
		break;
	};
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{

	switch (key) {
	case 'f':
	case 'F':
		if (fogMode == GL_EXP) {
			fogMode = GL_EXP2;
			printf("Fog mode is GL_EXP2\n");
		}
		else if (fogMode == GL_EXP2) {
			fogMode = GL_LINEAR;
			printf("Fog mode is GL_LINEAR\n");
		}
		else if (fogMode == GL_LINEAR) {
			fogMode = GL_EXP;
			printf("Fog mode is GL_EXP\n");
		}
		glFogi(GL_FOG_MODE, fogMode);
		glutPostRedisplay();
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}


int main(int argc, char* argv[])
{
	GLfloat plane[4];
	GLfloat v0[3], v1[3], v2[3];

	glutInit(&argc, argv);
	glutInitWindowSize(800, 800);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL | GLUT_DOUBLE | GLUT_SINGLE);
	(void)glutCreateWindow("3D Project");
	//glutDisplayFunc(redrawInterior);
	glutDisplayFunc(redrawExterior);
	glutKeyboardFunc(processNormalKeys);

	init();


	// draw a perspective scene 
	glMatrixMode(GL_PROJECTION);
	glFrustum(-100., 100., -100., 100., 320., 640.);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);

	// -------- make shadow matrices -> 3 points on floor    --------- // 

	v0[X] = -100.f;
	v0[Y] = -100.f;
	v0[Z] = -320.f;
	v1[X] = 100.f;
	v1[Y] = -100.f;
	v1[Z] = -320.f;
	v2[X] = 100.f;
	v2[Y] = -100.f;
	v2[Z] = -520.f;

	findplane(plane, v0, v1, v2);
	shadowmatrix(floorshadow, plane, lightpos);
	shadowmatrix(floorshadow_ext, plane, lightpos_ext);

	// -------- make shadow matrices -> 3 points on table    --------- // 

	v0[Y] = -86;
	v0[Z] = -320.f;
	v1[X] = 100.f;
	v1[Y] = -86;
	v1[Z] = -320.f;
	v2[X] = 100.f;
	v2[Y] = -86;
	v2[Z] = -520.f;

	findplane(plane, v0, v1, v2);
	shadowmatrix(tableshadow, plane, lightpos);

	// -------- make shadow matrices -> 3 points on right wall    --------- // 

	v0[X] = 100.f;
	v0[Y] = -100.f;
	v0[Z] = -320.f;
	v1[X] = 100.f;
	v1[Y] = -100.f;
	v1[Z] = -520.f;
	v2[X] = 100.f;
	v2[Y] = 100.f;
	v2[Z] = -520.f;

	findplane(plane, v0, v1, v2);
	shadowmatrix(rightwallshadow, plane, lightpos);


	// -------- make shadow matrices -> 3 points on left wall    --------- // 

	v0[X] = -100.f;
	v0[Y] = -100.f;
	v0[Z] = -320.f;
	v1[X] = -100.f;
	v1[Y] = -100.f;
	v1[Z] = -520.f;
	v2[X] = -100.f;
	v2[Y] = 100.f;
	v2[Z] = -520.f;

	findplane(plane, v0, v1, v2);
	shadowmatrix(leftwallshadow, plane, lightpos);

	int subMenId = glutCreateMenu(menu);
	glutAddMenuEntry("Textura 1", INTERIOR_T1);
	glutAddMenuEntry("Textura 2", INTERIOR_T2);

	int subExtMenId = glutCreateMenu(menu);
	glutAddMenuEntry("Textura 1", EXTERIOR_T1);
	glutAddMenuEntry("Textura 2", EXTERIOR_T2);
	glutAddMenuEntry("Ceata", EXTERIOR_T3);

	glutCreateMenu(menu);
	glutAddSubMenu("Interior", subMenId);
	glutAddSubMenu("Exterior", subExtMenId);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//glutKeyboardFunc(keyboard);

	// remove back faces to speed things up 
	glCullFace(GL_BACK);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glutMainLoop();
	return 0;
}
