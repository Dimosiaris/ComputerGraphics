//alec
#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#endif
#include <GL/glew.h>  // need to be included before gl too avoid errors
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/freeglut.h>
#include <time.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <windows.h>






#define RED 0
#define BLUE 1
#define ROCK 2
#define PAPER 3
#define SCISSORS 4
#define BOMB 5 
#define BLACK 6
#define EXPLOSION 7

// Constant definitions for Menus

#define STARTGAME 1
#define EXIT 2


GLuint textures[8]; //the array for our texture
GLdouble ox = 0.0, oy = 0.0, oz = 0.0; // coordinates

// The previous choice of the matrix
int prevX = -1;
int prevZ = -1;

// actual vector representing the camera's direction
float lx = 1.0f, lz = 1.0f;

// XZ position of the camera
float x = 9.5f, z = 10.5f;

// the key states. These variables will be zero
//when no key is being presses
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

// The counter of clicks
int clicks = 0;

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

void computeExplosion(int x , int z , int  color , int axis);
void explosion(int x , int z);	
void checkExplosion(int z, int x);
void checkClick(float oz, float ox);


//alec


void mySleep(int sleepMs)
{
#ifdef LINUX
	sleep();
#endif
#ifdef WINDOWS
	Sleep();
#endif
}

//alec : 
void explosion(int x, int z) {
	//cubes[z][x].color = EXPLOSION;
	//mySleep(2);
	cubes[x][z].color = BLACK;
}

//alec 5% chance for bomb
int textureChooser() {
	float a = rand() / float(RAND_MAX);
	if (a < 0.198)
		return 0;
	else if (a < 0.396)
		return 1;
	else if (a < 0.594)
		return 2;
	else if (a < 0.792)
		return 3;
	else if (a < 0.99)
		return 4;
	return 5;
}

// The world coordinates that the mouse returns to us to matrix position for x
int worldToMatrixCoordx(float x) {
	//x -= 1;
	int temp = (int)floor(x - ((floor(x / 1.5) / 2)));	// Copyrights Dimosiaris
	return temp;
}
// The world coordinates that the mouse returns to us to matrix position for z
int worldToMatrixCoordz(float z) {
	z += 23;
	return (int)floor(z - ((floor(z / 1.5) / 2))); // Copyrights Dimosiaris
}

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
				randomint = textureChooser(); //alec for sure
				cubes[i][j].color = randomint;
				cubes[i][j].row = i;
				cubes[i][j].column = j;
			}
		}
		//alec : changed the names of the files

		loadTextureFromFile("scissors.jpg", SCISSORS);
		loadTextureFromFile("rock.jpg", ROCK);
		loadTextureFromFile("paper.jpg", PAPER);
		loadTextureFromFile("red.bmp", RED);
		loadTextureFromFile("blue.bmp", BLUE);

		loadTextureFromFile("bomb.jpg", BOMB); //alec :BONUS
		loadTextureFromFile("explosion.jpg", EXPLOSION);
		loadTextureFromFile("black.jpg", BLACK);
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
	if (color == -1) {
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
				drawCube(-1); //color is the beige
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

void Mouse(int button, int state, int x, int y) {

	GLint viewport[4];

	GLdouble modelview[16], projection[16];

	GLfloat wx = x, wy, wz;

	if (state != GLUT_DOWN)

		return;

	if (button == GLUT_RIGHT_BUTTON)

		exit(0);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		glGetIntegerv(GL_VIEWPORT, viewport); // returns x, y, height, width

		y = viewport[3] - y;	// height - y

		wy = y;

		glGetDoublev(GL_MODELVIEW_MATRIX, modelview); // for the transformations

		glGetDoublev(GL_PROJECTION_MATRIX, projection); // what we see

		glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz); // get the z

		gluUnProject(wx, wy, wz, modelview, projection, viewport, &ox, &oy, &oz); // the final coordinates

		//glutPostRedisplay();
		printf("----------------------------------------------------------\n");
		printf("(x, y, z) in world coordinates is:(%f, %f)\n", ox, oz);
		checkClick(ox, oz);
		return;
	}


}
// Checks if the click was the first one, if the cubes selected are in the same column or row and if so calls the explosion function
void checkClick(float ox, float oz) {
	// First convert to matrix coordinates
	int x, z, checkX, checkZ;
	x = worldToMatrixCoordx(ox);
	z = worldToMatrixCoordz(oz);
	printf("x: %d\n", x);
	printf("z: %d\n", z);
	printf("color: %d\n", cubes[x][z].color);
	if (!gameStarted) {
		printf("Start the game first\n");
		prevX = -1;
		prevZ = -1;
		return;
	}
	if (x < 0 || x > 14 || z < 0 || z > 14) {
		printf("Out of bounds!\n");
		prevX = -1;
		prevZ = -1;
		return;
	}
	if (prevX == -1 && prevZ == -1) {	// this is the first click for every set of clicks
		prevX = x;
		prevZ = z;
		clicks++;
	}
	else { // This is the second click for every set of clicks
		checkX = abs(prevX - x);
		checkZ = abs(prevZ - z);
		if (checkZ == 1 && checkX == 1) { // they are in the same diagonal, no switch
			prevX = -1;
			prevZ = -1;
			printf("Switch not allowed, please try a set of cubes that are in the same column or row");
			return;
		}
		else if (checkZ != 1 && checkX != 1) { // Not in the same row or column
			prevX = -1;
			prevZ = -1;
			printf("Switch not allowed, please try a set of cubes that are in the same column or row");
			return;
		}
		else if (checkZ == 1 || checkX == 1) { // They are in the same column, change rows		
			int temp = cubes[prevZ][prevX].color;
			cubes[prevZ][prevX].color = cubes[z][x].color;
			cubes[z][x].color = temp;
		}
		
		checkExplosion(z, x);
		checkExplosion(prevZ, prevX);
		prevX = -1;
		prevZ = -1;
		moves++;
		return;
	}
}

// TODO CREATE THE IMPLEMENTATION
// The function that checks for explosion
void checkExplosion(int x, int z) {
	int *color;
	color = &cubes[x][z].color;
	printf("_______________________________________________________________\n");
	printf("Does it explode for cube[%d][%d] with color: %d\n", x, z, *color);

	if ((x + 1) < 15  && cubes[x + 1][z].color == *color &&  (x - 1) >=0 && cubes[x - 1][z].color == *color) { //Checking 1 on the right and 1 n the left
		printf("1) Yes  cube[%d][%d] with color: %d exploded with 1 on the right and 1 on the left\n", x, z, *color);
		computeExplosion(x, z, *color, 0);
		
	}
	if ((z + 1) < 15 && cubes[x][z + 1].color == *color && (z - 1) >= 0 && cubes[x][z - 1].color == *color) { //Checking 1 up and 1 down
		printf("2) Yes  cube[%d][%d] with color: %d exploded with 1 up and 1 down\n", x, z, *color);
		computeExplosion(x, z, *color, 1);
		
	}
	if ((x + 2) < 15 && cubes[x + 1][z].color == *color && cubes[x + 2][z].color == *color) { //Checking 2 right
		printf("3) Yes  cube[%d][%d] with color: %d exploded with 2 on the right\n", x, z, *color);
		computeExplosion(x + 1, z, *color, 0);
		
	}
	if (x - 2 >= 0 && cubes[x - 1][z].color == *color && cubes[x - 2][z].color == *color) { //Checking 2 left
		printf("4) Yes  cube[%d][%d] with color: %d exploded with 2 on the left\n", x, z, *color);
		computeExplosion(x - 1, z, *color, 0);
		
	}
	if ((z + 2) < 15 && cubes[x][z + 1].color == *color && cubes[x][z + 2].color == *color) { //Checking 2 up
		printf("5) Yes  cube[%d][%d] with color: %d exploded with 2 up\n", x, z, *color);
		computeExplosion(x, z + 1, *color, 1);
		
	}
	if ((z - 2) >= 0 && cubes[x][z - 1].color == *color && cubes[x][z - 2].color == *color) { //Checking 2 down
		printf("6) Yes  cube[%d][%d] with color: %d exploded with 2 down\n", x, z, *color);
		computeExplosion(x, z - 1, *color, 1);
		
	}
}
// The function that computes the actual explosion
void computeExplosion(int x, int z, int color , int axis) { //axis is 0 horizontal trio  and 1 for vertical trio
	int i, j;

	//LEVEL 0
	if (axis = 0) { 
		printf("LEVEL 0: horizontal");
		explosion(x, z);
		explosion(x + 1, z);
		explosion(x - 1, z);
	}
	else if (axis = 1) { 
		printf("LEVEL 0: vertical");
		explosion(x , z);
		explosion(x , z + 1);
		explosion(x , z - 1);
	}
		
	/*
	if (axis == 0) {
		printf("########HORIZONTAL##########\n");
		for (i = x - 2; i <= x + 2; i++) {
			for (j = z - 1; j <= z + 1; j++) {
				switch (*color)
				{
				case RED:
					break;
				case BLUE:
					break;
				case ROCK:
					if (cubes[i][j].color == PAPER) {
						break;
					}
					explosion(i, j);
					break;
				case PAPER:
					if (cubes[i][j].color == SCISSORS) {
						break;
					}
					explosion(i, j);
					break;
				case SCISSORS:
					if (cubes[i][j].color == PAPER) {
						break;
					}
					explosion(i, j);
					break;
				default:
					break;
				}
			}
		}
	}
	if (axis == 1) {
		printf("@@@@@@@VERTICAL@@@@@@@@@@\n");
		for (i = z - 2; i <= z + 2; i++) {
			for (j = x - 1; j <= x + 1; j++) {
				switch (*color)
				{
				case RED:
					break;
				case BLUE:
					break;
				case ROCK:
					if (cubes[i][j].color == PAPER) {
						break;
					}
					explosion(i, j);
					break;
				case PAPER:
					if (cubes[i][j].color == SCISSORS) {
						break;
					}
					explosion(i, j);
					break;
				case SCISSORS:
					if (cubes[i][j].color == PAPER) {
						break;
					}
					explosion(i, j);
					break;
				default:
					break;
				}
			}
		}
	}
	*/
	return;
}



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


	// init Menus
	createPopupMenus();

	initGL();

	glutMouseFunc(Mouse);

	// enter GLUT event processing cycle
	glutMainLoop();



	return 1;
}