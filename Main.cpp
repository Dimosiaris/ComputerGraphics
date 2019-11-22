#include <GL/glew.h>  // need to be included before gl too avoid errors
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <math.h>
#include <stdio.h>
#include <GL/freeglut.h>
#include <time.h>



// angle of rotation for the camera direction
float angle = 1.0f;

// actual vector representing the camera's direction
float lx = 1.0f, lz = 1.0f;

// XZ position of the camera
float x = 9.5f, z = 10.5f;

// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMoveX = 0;
float deltaMoveZ = 0;
int xOrigin = -1;

// Constant definitions for Menus

#define STARTGAME 1
#define EXIT 2



// Pop up menu identifiers
int mainMenu;

// scale of snowman
float scale = 1.0f;

// menu status
int menuFlag = 0;

void initGL() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClearDepth(1.0f);                   // Set background depth to farthest
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void drawCube() {

	glScalef(scale, scale, scale);
	glColor3f(0.82f, 0.71f, 0.55f);
	glTranslatef(1.0f, 0.0f, 0.0f);
	glutSolidCube(1.0f);
}

void computeUpDownPos(float deltaMoveX) {

	x += deltaMoveX * lx * 0.1f;
}
void computeLeftRightPos(float deltaMoveZ) {

	z += deltaMoveZ * lz * 0.1f;
}


void renderScene(void) {

	if (deltaMoveZ)
		computeUpDownPos(deltaMoveZ);
	if (deltaMoveX)
		computeLeftRightPos(deltaMoveX);

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	// Set the camera
	gluLookAt(x, 35.0f, z,
		x + lx, 1.0f, z ,
		0.0f, 1.0f, 0.0f);

	// Draw ground

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();

	glColor3f(1.0f, 0.0f, 0.0f);
	char textMoves[20];
	sprintf_s(textMoves, "MOVES:");

	// Draw 225 cubes
	int randomint;
	glColor3f(245.0f, 222.0f, 179.0f);
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			glPushMatrix();
			glTranslatef(j * 1.5f, 0.0f, 0.0f);
			randomint = (rand() % 4) + 1;	// TODO pass the randomint as a parameter to drawCube
			drawCube();
			glPopMatrix();
		}
		glTranslatef(0.0f, 0.0f, 1.5f);
	}
	glutSwapBuffers();
}

// -----------------------------------
//             KEYBOARD
// -----------------------------------

void processNormalKeys(unsigned char key, int xx, int yy) {

	glutSetMenu(mainMenu);
	switch (key) {
	case 27:
		glutDestroyMenu(mainMenu);
		printf("EXIT");
		exit(0);
		break;

	}
	if (key == 27)
		exit(0);
}

void pressKey(int key, int xx, int yy) {

	switch (key) {
	case GLUT_KEY_UP: deltaMoveZ = 0.1f; break;
	case GLUT_KEY_DOWN: deltaMoveZ = -0.1f; break;
	case GLUT_KEY_RIGHT: deltaMoveX = 0.1f; break;
	case GLUT_KEY_LEFT: deltaMoveX = -0.1f; break;
	}
}

void releaseKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_UP: deltaMoveZ = 0; break;
	case GLUT_KEY_DOWN: deltaMoveZ = 0; break;
	case GLUT_KEY_RIGHT: deltaMoveX = 0; break;
	case GLUT_KEY_LEFT: deltaMoveX = 0; break;
	}
}

// -----------------------------------
//             MOUSE
// -----------------------------------

/*

void mouseMove(int x, int y) {

	// this will only be true when the left button is down
	if (xOrigin >= 0) {

		// update deltaAngle
		deltaAngle = (x - xOrigin) * 0.001f;

		// update camera's direction
		lx = sin(angle + deltaAngle);
		lz = -cos(angle + deltaAngle);
	}
}


void mouseButton(int button, int state, int x, int y) {

	// only start motion if the left button is pressed
	if (button == GLUT_LEFT_BUTTON) {

		// when the button is released
		if (state == GLUT_UP) {
			angle += deltaAngle;
			xOrigin = -1;
		}
		else {// state = GLUT_DOWN
			xOrigin = x;
		}
	}
}

*/

// -----------------------------------
//             MENUS
// -----------------------------------


// MENU HANDLER
void processMainMenu(int option) {

	switch (option) {

	case STARTGAME: break;	// TODO ATTACH A FUCTION THAT WILL CALL THE ACTUAL GAME
	case EXIT:
		glutDestroyMenu(mainMenu); 
		exit(0);
		break;
	}
}

void createPopupMenus() {

	mainMenu = glutCreateMenu(processMainMenu);

	glutAddMenuEntry("Start Game", STARTGAME);
	glutAddMenuEntry("Exit", EXIT);

	// attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}

// -----------------------------------
//             MAIN
// -----------------------------------

int main(int argc, char **argv) {

	// Initialize rand
	srand(time(NULL));

	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(250, 75);
	glutInitWindowSize(600, 600);
	glutCreateWindow("VRahapsa III");

	// register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);

	// BONUS

	// here are the two new functions
	//glutMouseFunc(mouseButton); // BONUS
	// glutMotionFunc(mouseMove); // Needed for mouseMove function BONUS		

	// OpenGL init
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// init Menus
	createPopupMenus();

	initGL();
	
	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}