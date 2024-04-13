#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cmath>
#include <gl/glut.h>
#include "VECTOR3D.h"
#include <iostream>
#include <random>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

const int vWidth = 1280;    // Viewport width in pixels
const int vHeight = 720;    // Viewport height in pixels

// zeppelin properties
float scale = 0.5;
float zepBodyWidth = 5*scale;
float zepBodyLength = 2.5* scale;
float zepBodyDepth = 2.5* scale;
float xPos = -10.0;
float yPos = 5.0;
float zPos = 10.0;
// zeppelin angle direction facing
float zepAngle = 45.0;
// propellor rotation
float propellorAngle = -40.0;
// Enemy Zeppelin
float enemyX = 10.0;
float enemyY = 5.0;
float enemyZ = -10.0;
float enemyAngle = 270.0;
float enemyPropAngle = -40.0;

// gluLookAt parameters to change view
float eyeX = 0.0;
float eyeY = 6.0;
float eyeZ = 22.0;
float lookAtX = 0.0;
float lookAtY = 0.0;
float lookAtZ = 0.0;

// boolean for view
bool isFirstPerson = false;

// missile stuff
// the idea is when space bar pressed, missile becomes active
// while the missile is active, it will move in the direction it was shot in for a duration then despawn
bool isMissileActive = false;
float missileDuration;
float missileAngle = zepAngle;
bool isEnemyMissileActive = false;
float enemyMissileDuration;
float enemyMissileAngle = enemyAngle;
float missileX = 0.0;
float missileY = 0.0;
float missileZ = 0.0;
float enemyMissileX = 0.0;
float enemyMissileY = 0.0;
float enemyMissileZ = 0.0;
bool enemyAlive = true;
// Ground Mesh material
GLfloat groundMat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat groundMat_specular[] = { 0.01, 0.01, 0.01, 1.0 };
GLfloat groundMat_diffuse[] = { 0.4, 0.4, 0.7, 1.0 };
GLfloat groundMat_shininess[] = { 1.0 };

// Lighting/shading and material properties 
GLfloat zepBody_mat_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat zepBody_mat_specular[] = { 0.45f,0.55f,0.45f,1.0f };
GLfloat zepBody_mat_diffuse[] = { 0.61f,0.42f,0.63f,1.0f };
GLfloat zepBody_mat_shininess[] = { 32.0F };

GLfloat enemyBody_mat_diffuse[] = { 0.61f,0.14f,0.14f,1.0f };
GLfloat missile_diffuse[] = { 0.61f,0.14f,0.14f,1.0f };

// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Mouse button
int currentButton;


// Prototypes for functions
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void animationHandler(int param);
void drawBuilding();
void drawZeppelin();
void drawEnemy();
void drawMissile();
void drawEnemyMissile();
void draw();

// Generate a random number within the specified range
int generateRandomNumber(int x, int y) {

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distribution(x, y);
	int random_number = distribution(gen);
	return random_number;
}

int main(int argc, char** argv)
{

	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Zeppelin");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	glutTimerFunc(16, animationHandler, 0);
	// Start event loop, never returns
	glutMainLoop();

	return 0;
}

// Set up OpenGL
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void drawGround()
{
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, groundMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundMat_shininess);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-30.0f, -10.0f, -30.0f);
	glVertex3f(-30.0f, -10.0f, 30.0f);
	glVertex3f(30.0f, -10.0f, 30.0f);
	glVertex3f(30.0f, -10.0f, -30.0f);
	glEnd();
	glPopMatrix();
}

// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	//gluLookAt(0.0, 100.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	gluLookAt(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, 0.0, 1.0, 0.0);

	drawGround();
	

	// Apply modelling transformations M to move
	// Current transformation matrix is set to IV, where I is identity matrix
	// CTM = IV
	draw();
	printf("x: %.2f, y: %.2f, z: %.2f, Angle: %.2f\n", xPos, yPos, zPos, zepAngle); 
	
	glutSwapBuffers();   // Double buffering, swap buffers
}


void draw()
{
	glPushMatrix();
		glTranslatef(xPos, yPos, zPos);
		glRotatef(zepAngle, 0.0, 1.0, 0.0);
		glTranslatef(-xPos, -yPos, -zPos);
		drawZeppelin();
	glPopMatrix();
	if (enemyAlive) {
		glPushMatrix();
			glTranslatef(enemyX, enemyY, enemyZ);
			glRotatef(enemyAngle, 0.0, 1.0, 0.0);
			glTranslatef(-enemyX, -enemyY, -enemyZ);
			drawEnemy();
		glPopMatrix();
	}
	drawBuilding();
	if (isMissileActive) {
		drawMissile();
	}
	if (isEnemyMissileActive) {
		drawEnemyMissile();
	}


}

void drawMissile() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, zepBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zepBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, zepBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, zepBody_mat_shininess);

	glPushMatrix();
		glTranslatef(missileX, missileY, missileZ);
		glutSolidSphere(.25, 20.0, 20.0);
	glPopMatrix();
}

void drawEnemyMissile() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, zepBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zepBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, missile_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, zepBody_mat_shininess);

	glPushMatrix();
		glTranslatef(enemyMissileX, enemyMissileY, enemyMissileZ);
		glutSolidSphere(.25, 20.0, 20.0);
	glPopMatrix();
}


// all the buildings on the ground
void drawBuilding() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, zepBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zepBody_mat_specular);
	GLfloat diffuseColor[] = { 0.62f, 0.73f, 0.73f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
	glMaterialfv(GL_FRONT, GL_SHININESS, zepBody_mat_shininess);

	glPushMatrix();
	glTranslatef(-30.0, -9.0, -30.0); //put at ground level
	for (int i = 1; i < 6; i++) {

		glTranslatef(10.0, 0.0, 0.0);
		glPushMatrix();
		for (int j = 1; j < 6; j++) {
			glTranslatef(0.0,0.0,10.0);
			glPushMatrix();
			glScalef(1.0, ((i+j)%3+1) * 3.0, 1.0);
			glutSolidCube(2.0);
			glPopMatrix();
		}
		glPopMatrix();
		


	}
	glPopMatrix();


}

//Hierarchal Levels:
//	Body
//	Cabin		+ Fins							+ Propellor
//			Horizontal Fins + Vertical Fins		Shaft + Blades
//
void drawZeppelin() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, zepBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zepBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, zepBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, zepBody_mat_shininess);

	// Draw Zeppelin
	glPushMatrix();
		glTranslatef(xPos, yPos, zPos);
		glScalef(zepBodyWidth, zepBodyLength, zepBodyDepth);
		glutSolidSphere(1.0, 20, 20); // Draw Zeppelin Body

		// Cabin
		glPushMatrix(); 
			glTranslatef(0, -1, 0); 
			glScalef(0.5,0.5,0.5); 
			glutSolidCube(1.0); // Draw Cabin
		glPopMatrix();

		// Fins
		glPushMatrix();
			glTranslatef(-0.9, 0.0, 0.0);
			glRotatef(90, 0.0, 1.0, 0.0);

			// Horizontal Fins
			glPushMatrix();
				glScalef(1, 0.05, 0.75);
				glutSolidCone(1, 1, 20, 20);
			glPopMatrix();

			// Vertical Fins
			glPushMatrix();
				glScalef(0.05, 1, 0.75);
				glutSolidCone(1, 1, 20, 20);
			glPopMatrix();
		glPopMatrix();

		// Propellor (Shaft + Blades)
		glPushMatrix();
			glTranslatef(-0.5, 0, 0); // Propellor Position

			// Propellor Shaft
			glPushMatrix();
				glRotatef(270, 0.0, 1.0, 0.0);
				glScalef(0.85, 0.85, 0.75);
				glutSolidCone(1, 1, 20, 20);
			glPopMatrix();

			glTranslatef(-0.7, 0, 0); // offset for Propellor Blades

			// Propellor Blade 1
			glPushMatrix();
				glRotatef(propellorAngle, 1.0, 0.0, 0.0);
				glScalef(0.05, 0.02, 0.5); // Propellor Blade Scale
				glutSolidSphere(1.0, 20, 20);
			glPopMatrix();

			// Propellor Blade 2
			glPushMatrix();
				glRotatef(propellorAngle+90, 1.0, 0.0, 0.0);
				glScalef(0.05, 0.02, 0.5); // Propellor Blade Scale
				glutSolidSphere(1.0, 20, 20);
			glPopMatrix();

		glPopMatrix();

	glPopMatrix();

}

void drawEnemy() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, zepBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zepBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, enemyBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, zepBody_mat_shininess);

	// Draw Zeppelin
	glPushMatrix();
		glTranslatef(enemyX, enemyY, enemyZ);
		glScalef(zepBodyWidth, zepBodyLength, zepBodyDepth);
		glutSolidSphere(1.0, 20, 20); // Draw Zeppelin Body

		// Cabin
		glPushMatrix(); 
			glTranslatef(0, -1, 0); 
			glScalef(0.5,0.5,0.5); 
			glutSolidCube(1.0); // Draw Cabin
		glPopMatrix();

		// Fins
		glPushMatrix();
			glTranslatef(-0.9, 0.0, 0.0);
			glRotatef(90, 0.0, 1.0, 0.0);

			// Horizontal Fins
			glPushMatrix();
				glScalef(1, 0.05, 0.75);
				glutSolidCone(1, 1, 20, 20);
			glPopMatrix();

			// Vertical Fins
			glPushMatrix();
				glScalef(0.05, 1, 0.75);
				glutSolidCone(1, 1, 20, 20);
			glPopMatrix();
		glPopMatrix();

		// Propellor (Shaft + Blades)
		glPushMatrix();
			glTranslatef(-0.5, 0, 0); // Propellor Position

			// Propellor Shaft
			glPushMatrix();
				glRotatef(270, 0.0, 1.0, 0.0);
				glScalef(0.85, 0.85, 0.75);
				glutSolidCone(1, 1, 20, 20);
			glPopMatrix();

			glTranslatef(-0.7, 0, 0); // offset for Propellor Blades

			// Propellor Blade 1
			glPushMatrix();
				glRotatef(enemyPropAngle, 1.0, 0.0, 0.0);
				glScalef(0.05, 0.02, 0.5); // Propellor Blade Scale
				glutSolidSphere(1.0, 20, 20);
			glPopMatrix();

			// Propellor Blade 2
			glPushMatrix();
				glRotatef(enemyPropAngle+90, 1.0, 0.0, 0.0);
				glScalef(0.05, 0.02, 0.5); // Propellor Blade Scale
				glutSolidSphere(1.0, 20, 20);
			glPopMatrix();

		glPopMatrix();

	glPopMatrix();

}



// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
}

// Switch Perspective: true = bird's eye, false = first person
void updateView(bool v) {
	if (v == true) {
		eyeX = 0.0;
		eyeY = 40.0;
		eyeZ = 0.1;
		lookAtX = 0.0;
		lookAtY = 0.0;
		lookAtZ = 0.0;
	}
	else { //first person
		eyeX = xPos + 2.8 * std::cos(zepAngle * (M_PI / 180));//0.0;
		eyeY = yPos;//6.0;
		eyeZ = zPos - 2.8 * std::sin(zepAngle * (M_PI / 180));//22.0;
		lookAtX = eyeX + 1.0 * std::cos(zepAngle * (M_PI / 180));
		lookAtY = yPos;
		lookAtZ = eyeZ - 1.0 * std::sin(zepAngle * (M_PI / 180));
	}	
}

void move(float direction, bool isEnemy) {
	if (isEnemy) {
		enemyX += 0.05 * std::cos(direction * (M_PI / 180));
		enemyZ -= 0.05 * std::sin(direction * (M_PI / 180));
	}
	else {
		xPos += 0.5 * std::cos(direction * (M_PI / 180));
		zPos -= 0.5 * std::sin(direction * (M_PI / 180));
	}
	// if out of bounds, move to other side (kind of like pacman?)
	if (xPos > 22.0) xPos = -22.0;
	if (zPos > 22.0) zPos = -22.0;
	if (xPos < -22.0) xPos = 22.0;
	if (zPos < -22.0) zPos = 22.0;
	if (enemyX > 22.0) enemyX = -22.0;
	if (enemyZ > 22.0) enemyZ = -22.0;
	if (enemyX < -22.0) enemyX = 22.0;
	if (enemyZ < -22.0) enemyZ = 22.0;


}

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		move(zepAngle, false);
		break;
	case 'v':
		isFirstPerson = not isFirstPerson;
		break;
	case 32: // ASCII value for space bar
		if (isMissileActive == false) {
			isMissileActive = true;
		}
		break;
	}
	glutPostRedisplay();   // Trigger a window redisplay
}




float n = 1.0;
bool playerHit() {
	if (isEnemyMissileActive) {
		if (enemyMissileX > (xPos - n) && enemyMissileX <(xPos + n) && enemyMissileY < yPos + 2.0 && enemyMissileY > yPos - 2.0 && enemyMissileZ > zPos - n && enemyMissileZ < zPos + n) {
			return true;
		}
	}
	return false;
}

bool enemyHit() {
	if (isMissileActive) {
		if (missileX > (enemyX - n) && missileX <(enemyX + n) && missileY < enemyY + 2.0 && missileY > enemyY - 2.0 && missileZ > enemyZ - n && missileZ < enemyZ + n) {
			return true;
		}
	}
	return false;
}

void manageCollision() {
	if (playerHit()) {
		yPos = -8.0;
	}
	if (enemyHit()) {
		enemyAlive = false;
	}
}

void updateMissile() {
	if (isMissileActive) {
		missileDuration += 30;
		missileX += 0.75 * std::cos(missileAngle * (M_PI / 180));
		missileZ -= 0.75 * std::sin(missileAngle * (M_PI / 180));
		if (missileDuration >= 1500) {
			isMissileActive = false;
			missileDuration = 0.0;
			missileX = xPos;
			missileY = yPos - 1.0;
			missileZ = zPos;
			missileAngle = zepAngle;
		}
	}
	else {
		missileX = xPos;
		missileY = yPos-1.0;
		missileZ = zPos;
		missileAngle = zepAngle;
	}
	//enemy missile
	if (isEnemyMissileActive) {
		enemyMissileDuration += 30;
		enemyMissileX += 0.25 * std::cos(enemyMissileAngle * (M_PI / 180));
		enemyMissileZ -= 0.25 * std::sin(enemyMissileAngle * (M_PI / 180));
		if (enemyMissileDuration >= 2000) {
			isEnemyMissileActive = false;
			enemyMissileDuration = 0.0;
			enemyMissileX = enemyX;
			enemyMissileY = enemyY - 1.0;
			enemyMissileZ = enemyZ;
			enemyMissileAngle = enemyAngle;
		}
	}
	else {
		enemyMissileX = enemyX;
		enemyMissileY = enemyY - 1.0;
		enemyMissileZ = enemyZ;
		enemyMissileAngle = enemyAngle;
	}
}

bool enemyZeppelinClose() {
	float distance = sqrt(pow(enemyX - xPos, 2) + pow(enemyY - yPos, 2) + pow(enemyZ - zPos, 2));
	if (distance < 12.0) {
		return true;
	}
	else {
		return false;
	}
}

int changeTimer = 0.0;
int deathTimer = 0.0;
void updateEnemy() {
	if (enemyAlive) {
		if (enemyZeppelinClose()) {
			enemyBody_mat_diffuse[1] = 0.14f;
			// first adjust elevation, then turn to face player
			enemyAngle = atan2(-(zPos - enemyZ), xPos - enemyX) * (180.0 / M_PI);
			if (enemyY <= yPos) {
				enemyY += 0.1;
			}
			else if (enemyY >= yPos+0.5) {
				enemyY -= 0.1;
			}
			else {
				// fire enemy missile
				enemyMissileAngle = enemyAngle;
				if (isEnemyMissileActive == false) {
					isEnemyMissileActive = true;
				}
			}
		}
		else {
			enemyBody_mat_diffuse[1] = 0.80f;
			changeTimer++;
			if ((changeTimer % 100) == 0) {
				enemyAngle = generateRandomNumber(0, 360);
			}
			move(enemyAngle, true);
		}
	}
	else {
		deathTimer++;
		if (deathTimer > 100) {
			enemyAlive = true;
			enemyX = generateRandomNumber(-18, 18);
			enemyY = 5.0;
			enemyZ = generateRandomNumber(-18, 18);
			deathTimer = 0;
		}
	}
	
}



void animationHandler(int param)
{
	enemyPropAngle += 10.0;
	propellorAngle += 10.0;
	updateView(isFirstPerson);
	updateMissile();
	updateEnemy();
	manageCollision();
	glutPostRedisplay();
	glutTimerFunc(30, animationHandler, 0);
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		// Handle the up arrow key press
		if (yPos < 13.0) yPos += 1.0;
		break;
	case GLUT_KEY_DOWN:
		// Handle the down arrow key press
		if (yPos > -8.0) yPos -= 1.0;
		break;
	case GLUT_KEY_LEFT:
		// Handle the left arrow key press
		zepAngle += 5.0;
		break;
	case GLUT_KEY_RIGHT:
		// Handle the right arrow key press
		zepAngle -= 5.0;
		break;
	default:
		break;
	}
	glutPostRedisplay();   // Trigger a window redisplay
}

// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

void mouseMotionHandler(int xMouse, int yMouse)
{
	glutPostRedisplay();   // Trigger a window redisplay
}

