
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
int prex = -1;
int prez = -1;

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

void computeExplosion(int m1, int m2, int m3, int n, int rc);
void explosion(cube cube);	// TODO CREATE THE IMPLEMENTATION
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
void explosion(int z, int x) {
	printf("BOOOOOOM\n");
	cubes[z][x].color = EXPLOSION;
	Sleep(200);
	cubes[z][x].color = BLACK;
}

//alec 5% chance for bomb
int textureChooser() {
	float a = rand() / float(RAND_MAX);
	if (a < 0.19)
		return 0;
	else if (a < 0.38)
		return 1;
	else if (a < 0.57)
		return 2;
	else if (a < 0.76)
		return 3;
	else if (a < 0.95)
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
	 
	
	glGenTextures(1, textures+i);
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

	// Prevent a divide by zero, when window is too short (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = (float)w * 1.0 / (float)h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();


	
	// Set the viewport to be the entire window
	glViewport(x, z, w, h);

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
	else{
		
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


	char textScore[20];
	sprintf_s(textScore, "Score: %d", score);
	glColor3f(1.0f, 0.30f, 1.0f);
	drawString(24.0, 0.0, 20.0, textScore);
	glColor3f(1.0f, 1.0f, 1.0f);
	

	// Draw 225 cubes
	int randomint = 0;
	if (gameStarted == 0) {
		glColor3f(245.0f, 222.0f, 179.0f);
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 15; j++) {
				glPushMatrix();
				glTranslatef(j * 1.5f, 0.0f, 0.0f);
				drawCube(-1);
				glPopMatrix();
			}
			glTranslatef(0.0f, 0.0f, 1.5f);
		}
	} 
	else{
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
	glPopMatrix();

	glPushMatrix();

	glDepthMask(GL_FALSE);
	
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
		printf("(x, y, z) in world coordinates is:(%f, %f)\n", ox, oz);
		checkClick(oz, ox);
		return;
	}
	

}
// Checks if the click was the first one, if the cubes selected are in the same column or row and if so calls the explosion function
void checkClick(float oz, float ox) {
	// First convert to matrix coordinates
	int x, z, checkx, checkz;
	printf("TEST@");
	x = worldToMatrixCoordx(ox);
	z = worldToMatrixCoordz(oz);
	printf("x: + %d\n", x);
	printf("z: + %d\n", z);
	if (prex == -1 && prez == -1) {	// this is the first click for every set of clicks
		prex = x;
		prez = z;
		clicks++;
	}
	else { // This is the second click for every set of clicks
		checkx = abs(prex - x);
		checkz = abs(prez - z);
		if (checkz == 1 && checkx == 1) { // they are in the same diagonal, no switch
			prex = -1;
			prez = -1;
			printf("Switch not allowed, please try a set of cubes that are in the same column or row");
			return;
		}
		else if(checkz != 1 && checkx != 1){ // Not in the same row or column
			prex = -1;
			prez = -1;
			printf("Switch not allowed, please try a set of cubes that are in the same column or row");
			return;
		}
		else if (checkz == 1) { // They are in the same column, change rows
			printf("cubes[%d][%d] = %ld", prez, prex, cubes[prez][prex]);
			printf("CHECK!");		
			int temp = cubes[prez][prex].color;
			cubes[prez][prex].color = cubes[z][x].color;
			cubes[z][x].color = temp;
			
		}
		else if (checkx == 1) { // They are in the same row, change columns
			printf("CHECK!2");
			printf("pre x :%d, z: %d", prex, prez);
			int temp1 = cubes[prez][prex].color;
			cubes[prez][prex].color = cubes[z][x].color;
			cubes[z][x].color = temp1;
		}
		prex = -1;
		prez = -1;
		checkExplosion(z, x);
		checkExplosion(prez, prex);
		moves++;
	}
}

// TODO CREATE THE IMPLEMENTATION
// The function that checks for explosion
void checkExplosion(int z, int x) {
	int color = cubes[z][x].color;
	printf("COLORADO: %d\n", cubes[z][x].color);
	if ((z + 2) < 15 && cubes[z + 1][x].color == color && cubes[z + 2][x].color == color) {
		printf("BOOM?\n");
		computeExplosion(z, z + 1, z + 2, x, 1); // x is a column 
	}
	if (z - 2 >= 0 && cubes[z - 1][x].color == color && cubes[z - 2][x].color == color) {
		printf("BOOM?\n");
		computeExplosion(z - 2, z - 1, z, x, 1); // x is a column
	}
	if ((x + 2) < 15 && cubes[z][x + 1].color == color && cubes[z][x + 2].color == color) {
		printf("BOOM?\n");
		computeExplosion(x, x + 1, x + 2, z, 0); // z is a row
	}
	if ((x - 2) >= 0 && cubes[z][x - 1].color == color && cubes[z][x - 2].color == color) {
		printf("BOOM?\n");
		computeExplosion(x, x - 1, x - 2, z, 0); // z is a row
	}
}
// The function that computes the actual explosion
void computeExplosion(int m1, int m2, int m3, int n, int rc) { // rc = row(0)->n or column(1)->n and m is the opposite of the n
	if (rc == 0) {	// n is a row
		int z = n;		// The z
		int xl = m1;	// The left x in a set of three
		int xm = m2;	// The middle x in a set of three
		int xr = m3;	// The right x in a set of three
		int color = cubes[xm][z].color;
		printf("YO: %d", color);
		int i, j;
		printf("COLOR: %d\n", color);
		for (i = z - 3; i <= z + 3; i++) {	// levels 2,3
			for (j = xl - 3; j <= xr + 3; j++) {
				if (i >= z - 1 && i <= z + 1) {} // do nothing
				else if (j >= xl - 1 && j <= xr + 1) {} // do nothing
				else if (i >= 0 && i < 15 && j >= 0 && j < 15) { // we are at level 2 and 3
					switch (color) {
					case(RED):
						break;
					case(BLUE):
						break;
					case(ROCK):
						if (cubes[i][j].color == SCISSORS) {
							explosion(i,j);
						}
						break;
					case(PAPER):
						if (cubes[i][j].color == ROCK) {
							explosion(i, j);
						}
						break;
					case(SCISSORS):
						if (cubes[i][j].color == PAPER) {
							explosion(i, j);
						}
						break;
					}
				}
			}
		}
		for (i = z - 1; i <= z + 1; i++) {	// level 0, 1
			for (j = xl - 1; j <= xr + 1; j++) {
				if (i == z && j >= xl && j <= xr) { // level 0
					printf("DID IT\n");
					explosion(i, j);
				}
				else {	// level 1
					switch (color) {
					case(RED):
						break;
					case(BLUE):
						break;
					case(ROCK):
						if (cubes[i][j].color == PAPER) {
							break;
						}
						else {
							explosion(i, j);
						}
						break;
					case(PAPER):
						if (cubes[i][j].color == SCISSORS) {
							break;
						}
						else {
							explosion(i, j);
						}
						break;
					case(SCISSORS):
						if (cubes[i][j].color == ROCK) {
							break;
						}else{
							explosion(i, j);
						}
						break;
					}
				}
			}
		}
	}
	else if (rc == 1) {	// n is a column
		int x = n;
		int zu = m1;	// The up z in a set of three
		int zm = m2;	// The middle z in a set of three
		int zd = m3;	// The down z in a set of three
		int color = cubes[zm][x].color;
		int i, j;

		for (i = zd - 3; i <= zu + 3; i++) {	// levels 2,3
			for (j = x - 3; j <= x + 3; j++) {
				if (j >= x - 1 && j <= x + 1) {} // do nothing
				else if (i >= zd - 1 && i <= zu + 1) {} // do nothing
				else if (j >= 0 && j < 15 && i >= 0 && i < 15) { // we are at level 2 and 3
					switch (color) {
					case(RED):
						break;
					case(BLUE):
						break;
					case(ROCK):
						if (cubes[i][j].color == SCISSORS) {
							explosion(i, j);
						}
						break;
					case(PAPER):
						if (cubes[i][j].color == ROCK) {
							explosion(i, j);
						}
						break;
					case(SCISSORS):
						if (cubes[i][j].color == PAPER) {
							explosion(i, j);
						}
						break;
					}
				}
			}
		}
		for (i = zd - 1; i <= zu + 1; i++) {	// level 0, 1
			for (j = x - 1; j <= x + 1; j++) {
				if (i == x && j >= zd && j <= zu) { // level 0
					printf("BOOM\n");
					explosion(i, j);
				}
				else {	// level 1
					switch (color) {
					case(RED):
						break;
					case(BLUE):
						break;
					case(ROCK):
						if (cubes[i][j].color == PAPER) {
							break;
						}
						else {
							explosion(i, j);
						}
						break;
					case(PAPER):
						if (cubes[i][j].color == SCISSORS) {
							break;
						}
						else {
							explosion(i, j);
						}
						break;
					case(SCISSORS):
						if (cubes[i][j].color == ROCK) {
							break;
						}
						else {
							explosion(i, j);
						}
						break;
					}
				}
			}
		}
	}
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