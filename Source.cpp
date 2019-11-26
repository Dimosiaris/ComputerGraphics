#include <GL/glew.h>  // need to be included before gl too avoid errors
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/freeglut.h>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

#define RED 0
#define BLUE 1
#define ROCK 2
#define PAPER 3
#define SCISSORS 4

// Constant definitions for Menus

#define STARTGAME 1
#define EXIT 2


GLuint textures[5]; //the array for our texture


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




// Pop up menu identifiers
int mainMenu;

// scale of cubes
float scale = 1.0f;

// menu status
int gameStarted = 0;

// textures initialized
int gameInitialized = 0;

// Moves of the player
int moves = 0;

// Score of the player
int score = 0;


// the main matrix of the game
struct cube {
	int row;
	int column;
	int color;
	float x;
	float z;
}cubes[15][15];




void drawString(float x, float y, float z, char* string) {
	glRasterPos3f(x, y, z);

	for (char* c = string; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);  // Updates the position
	}
}

void loadTextureFromFile(const char* filename, int i)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	//glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);


	glGenTextures(1, textures + i);
	glBindTexture(GL_TEXTURE_2D, textures[i]);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);


	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
		printf("Added texture %s \n", filename);
	}
	else
	{
		printf("Failed to load texture %s \n", filename);

	}
	//stbi_image_free(data);
}


void initGL() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClearDepth(1.0f);                   // Set background depth to farthest
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
	int randomint;
	if (gameInitialized == 0) {
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				randomint = (rand() % 5); //alec for sure
				cubes[i][j].color = randomint;
				cubes[i][j].row = i;
				cubes[i][j].column = j;
			}
		}
		//alec
		loadTextureFromFile("scissors.jpg", SCISSORS);
		loadTextureFromFile("rock.jpg", ROCK);
		loadTextureFromFile("paper.jpg", PAPER);
		loadTextureFromFile("red.bmp", RED);
		loadTextureFromFile("blue.bmp", BLUE);
		gameInitialized = 1;
	}
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

void FreeTexture(GLuint texture)
{
	glDeleteTextures(1, &texture);
}

void drawCube(int color) {
	if (color == 5) {
		glDisable(GL_TEXTURE_2D);
		glScalef(scale, scale, scale);
		glColor3f(0.82f, 0.71f, 0.55f);
		glTranslatef(1.5f, 0.0f, 0.0f);
		glutSolidCube(1.0f);
		return;
	}
	else {

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[color]);
		glBegin(GL_QUADS);				// start drawing the cube.
		
		//alec : kane ola ta 0.5 -> 0.9
		 // Front Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);	// Bottom Left Of The Texture and Quad
		glTexCoord2f(0.9f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);	// Bottom Right Of The Texture and Quad
		glTexCoord2f(0.9f, 0.9f); glVertex3f(0.5f, 0.5f, 0.5f);	// Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 0.9f); glVertex3f(-0.5f, 0.5f, 0.5f);	// Top Left Of The Texture and Quad

		// Back Face
		glTexCoord2f(0.9f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);	// Bottom Right Of The Texture and Quad
		glTexCoord2f(0.9f, 0.9f); glVertex3f(-0.5f, 0.5f, -0.5f);	// Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 0.9f); glVertex3f(0.5f, 0.5f, -0.5f);	// Top Left Of The Texture and Quad
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);	// Bottom Left Of The Texture and Quad

		// Top Face
		glTexCoord2f(0.0f, 0.9f); glVertex3f(-0.5f, 0.5f, -0.5f);	// Top Left Of The Texture and Quad
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, 0.5f, 0.5f);	// Bottom Left Of The Texture and Quad
		glTexCoord2f(0.9f, 0.0f); glVertex3f(0.5f, 0.5f, 0.5f);	// Bottom Right Of The Texture and Quad
		glTexCoord2f(0.9f, 0.9f); glVertex3f(0.5f, 0.5f, -0.5f);	// Top Right Of The Texture and Quad

		// Bottom Face
		glTexCoord2f(0.9f, 0.9f); glVertex3f(-0.5f, -0.5f, -0.5f);	// Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 0.9f); glVertex3f(0.5f, -0.5f, -0.5f);	// Top Left Of The Texture and Quad
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);	// Bottom Left Of The Texture and Quad
		glTexCoord2f(0.9f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);	// Bottom Right Of The Texture and Quad

		// Right face
		glTexCoord2f(0.9f, 0.0f); glVertex3f(0.5f, -0.5f, -0.5f);	// Bottom Right Of The Texture and Quad
		glTexCoord2f(0.9f, 0.9f); glVertex3f(0.5f, 0.5f, -0.5f);	// Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 0.9f); glVertex3f(0.5f, 0.5f, 0.5f);	// Top Left Of The Texture and Quad
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);	// Bottom Left Of The Texture and Quad

		// Left Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);	// Bottom Left Of The Texture and Quad
		glTexCoord2f(0.9f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);	// Bottom Right Of The Texture and Quad
		glTexCoord2f(0.9f, 0.9f); glVertex3f(-0.5f, 0.5f, 0.5f);	// Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 0.9f); glVertex3f(-0.5f, 0.5f, -0.5f);	// Top Left Of The Texture and Quad

		
		glEnd();

		glFlush();
		glDisable(GL_TEXTURE_2D);
	}
}

void computeUpDownPos(float deltaMoveX) {

	x += deltaMoveX * lx * 0.5f;
}
void computeLeftRightPos(float deltaMoveZ) {

	z += deltaMoveZ * lz * 0.5f;
}


void renderScene(void) {

	if (deltaMoveZ)
		computeUpDownPos(deltaMoveZ);
	if (deltaMoveX)
		computeLeftRightPos(deltaMoveX);

	glEnable(GL_DEPTH_TEST);
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);

	// Set the camera
	gluLookAt(x, 35.0f, z,
		x + lx, 1.0f, z,
		0.0f, 1.0f, 0.0f);

	// Draw ground

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(-100.0f, 0.0f, -100.0f);
	glVertex3f(-100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, 100.0f);
	glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();
	char textMoves[20];
	sprintf_s(textMoves, "Moves: %d", moves);
	glColor3f(1.0f, 0.30f, 1.0f);
	drawString(24.0, 0.0, 20.0, textMoves);
	glColor3f(1.0f, 1.0f, 1.0f);


	// Draw 225 cubes
	int randomint = 0;
	if (gameStarted == 0) {
		glColor3f(245.0f, 222.0f, 179.0f);
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				glPushMatrix();
				glTranslatef(j * 1.5f, 0.0f, 0.0f);
				drawCube(5);
				glPopMatrix();
			}
			glTranslatef(0.0f, 0.0f, 1.5f);
		}
	}
	else {
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				glPushMatrix();
				glTranslatef(j * 1.5f, 0.0f, 0.0f);
				drawCube(cubes[i][j].color);
				glPopMatrix();
			}
			glTranslatef(0.0f, 0.0f, 1.5f);
		}
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

	case STARTGAME:
		gameStarted = 1;
		break;	// TODO ATTACH A FUCTION THAT WILL CALL THE ACTUAL GAME
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

int main(int argc, char** argv) {

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