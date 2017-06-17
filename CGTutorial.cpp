#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


#include "shader.hpp"
#include "objects.hpp"
#include "objloader.hpp"
#include "texture.hpp"
#include "obj.h"


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

using namespace glm;
using namespace std;

/* GLOBAL VARIABLES */

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float angleX = 0.0; 
float angleY = 0.0; 
float angleZ = 0.0; 

float neigungX = 0.0;
float neigungU = 0.0;

glm::vec3 neigungXVec = glm::vec3(0, 1, 0);


// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;

bool keys[1024];

// Diese Drei Matrizen global (Singleton-Muster), damit sie jederzeit modifiziert und
// an die Grafikkarte geschickt werden koennen
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
glm::mat4 Door;

GLuint programID;
GLuint skyboxID;

float angle = 0.0;
GLuint textureBrick;
GLuint textureDoor;
GLuint textureGrass;
GLuint textureRoof;
GLuint textureWindow;
GLuint textureSky;


unsigned int textureID;
string m_fileNames[6];
GLuint m_textureObj;

/* CURSER VARIABLES */
bool enteredDoor = false;
bool rightPosition = false;
bool clicked = false;
bool initialised = false;

/* WRITE DOWN AN ERROR MESSAGE IN CONSOLE */
void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

/* CAMERA MOVES IN THE SCENE */
void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 0.01f;
	if (keys[GLFW_KEY_W])
	{
		cameraPos += cameraSpeed * cameraFront;
	}
	if (keys[GLFW_KEY_S])
	{
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (keys[GLFW_KEY_A])
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (keys[GLFW_KEY_D])
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (keys[GLFW_KEY_X])
	{
		angleX++;
	}
	if (keys[GLFW_KEY_Y])
	{
		angleY++;
	}
	if (keys[GLFW_KEY_Z])
	{
		angleZ++;
	}
	if (keys[GLFW_KEY_UP])
	{
		if (neigungX <= -5) {} //<--- magic number verkleinern 
		else {neigungX -= 0.1f; }
	}
	if(keys[GLFW_KEY_DOWN])
	{
		if (neigungX >= 5) {} //<--- magic number verkleiner
		else{neigungX += 0.1f; }
	}
}

/* DEFINES ACTIONS FOR THE PRESSED KEYS */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GLfloat cameraSpeed = 0.05f;

	if (action == GLFW_PRESS)
	{
		keys[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = false;
	}


	if (key == GLFW_KEY_W)
	{
		cameraPos += cameraSpeed * cameraFront;
	}
	if (key == GLFW_KEY_S)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (key == GLFW_KEY_A)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (key == GLFW_KEY_D)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}


	if (key == GLFW_KEY_X)
	{
		angleX++;
	}
	if (key == GLFW_KEY_Y)
	{
		angleY++;
	}
	if (key == GLFW_KEY_Z)
	{
		angleZ++;
	}

	if (keys[GLFW_KEY_UP])
	{
		if (neigungX <= -24.9) {}
		else { neigungX -= 0.1f; }
	}
	if (keys[GLFW_KEY_DOWN])
	{
		if (neigungX >= 24.9) {}
		else { neigungX += 0.1f; }
	}
}

/* CREATES THE VIEW(CAMERA); THE MODELS(OBJECTS) */
void sendMVP()
{
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform, konstant fuer alle Eckpunkte
	glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);

	/*  unterschiedliche Matrix wird hineingeladen für Shader*/
	glUniformMatrix4fv(glGetUniformLocation(programID, "M"), 1, GL_FALSE, &Model[0][0]); //
	glUniformMatrix4fv(glGetUniformLocation(programID, "V"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "P"), 1, GL_FALSE, &Projection[0][0]);
}

/* Defines the correct version */
void instantiateVersion()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}
 
/* changes the Curser icon ig the users curser is over the door */
static void curserPosionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if ((xpos > (SCREEN_WIDTH/2 - 80)) && (xpos < (SCREEN_WIDTH / 2 + 80)) && (ypos > (SCREEN_HEIGHT/2 - 60)) && (ypos < (SCREEN_HEIGHT/2 + 60)))
	{
		unsigned char pixels[16 * 16 * 4];
		memset(pixels, 0xff, sizeof(pixels));
		GLFWimage image;
		image.width = 16;
		image.height = 16;
		image.pixels = pixels;
		GLFWcursor *curser = glfwCreateCursor(&image, 0, 0);
		glfwSetCursor(window, curser);
		rightPosition = true;
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		rightPosition = false;
	}
}

/* checks whether the user has entered the window */
void curserEnterCallback(GLFWwindow* window, int entered)
{
	if (entered)
	{
		enteredDoor = true;
	}
	else 
	{
		enteredDoor = false;
	}
}

/* checks whether the user clicked the left button of the mouse*/
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		clicked = true;
	}
	else
	{
		clicked = false;
	}
}

/* will open the door if the user entered the window and the house and clicked on the door with the left mouse button */
void openDoor()
{
	if (clicked == true && enteredDoor == true &&rightPosition == true)
	{
		Door = glm::rotate(Door, 5.0f, glm::vec3(10, 0, 0));
		cout << "Door open" << endl;
	}
}

/* if the user press "esc" it will close the window */
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

enum { SKY_LEFT = 0, SKY_BACK, SKY_FRONT, SKY_TOP, SKY_BOTTOM, SKY_RIGHT};
unsigned int skybox[6];

void initSkybox()
{
	skybox[SKY_LEFT] = loadBMP_custom("left.bmp");
	skybox[SKY_BACK] = loadBMP_custom("back.bmp");
	skybox[SKY_RIGHT] = loadBMP_custom("right.bmp");
	skybox[SKY_FRONT] = loadBMP_custom("front.bmp");
	skybox[SKY_TOP] = loadBMP_custom("top.bmp");
	skybox[SKY_BOTTOM] = loadBMP_custom("bottom.bmp");
}

void killSkyBox() {
	glDeleteTextures(6, &skybox[0]);
}

void drawSkybox(float size) {

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, skybox[SKY_BACK]);
	glBegin(GL_QUADS);
	//back face
	glTexCoord2f(0,0);
	glVertex3f(size / 2, size / 2, size / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-size / 2, size / 2, size / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-size / 2, -size / 2, size / 2);
	glTexCoord2f(0, 1);
	glVertex3f(size / 2, -size / 2, size / 2);
	glEnd();
		
	glBindTexture(GL_TEXTURE_2D, skybox[SKY_LEFT]);
	glBegin(GL_QUADS);
	//left face
	glTexCoord2f(0,0);
	glVertex3f(-size / 2, size / 2, size / 2);
	glTexCoord2f(1,0);
	glVertex3f(-size / 2, size / 2, -size / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-size / 2, -size / 2, -size / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-size / 2, -size / 2, size / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[SKY_FRONT]);
	glBegin(GL_QUADS);
	//front face
	glTexCoord2f(1, 0);
	glVertex3f(size / 2, size / 2, -size / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-size / 2, size / 2, -size / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-size / 2, -size / 2, -size / 2);
	glTexCoord2f(1, 1);
	glVertex3f(size / 2, -size / 2, -size / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[SKY_RIGHT]);
	glBegin(GL_QUADS);
	//right face
	glTexCoord2f(0, 0);
	glVertex3f(size / 2, size / 2, size / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-size / 2, size / 2, size / 2);
	glTexCoord2f(1, 1);
	glVertex3f(-size / 2, size / 2, -size / 2);
	glTexCoord2f(0, 1);
	glVertex3f(size / 2, size / 2, -size / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[SKY_BOTTOM]);
	glBegin(GL_QUADS);
	//bottom face
	glTexCoord2f(1, 1);
	glVertex3f(size / 2, -size / 2, size / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-size / 2, -size / 2, size / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-size / 2, -size / 2, -size / 2);
	glTexCoord2f(1, 0);
	glVertex3f(size / 2, -size / 2, -size / 2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[SKY_TOP]);
	glBegin(GL_QUADS);
	//top face
	glTexCoord2f(1, 0);
	glVertex3f(size / 2, -size / 2, size / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-size / 2, -size / 2, size / 2);
	glTexCoord2f(0, 1);
	glVertex3f(-size / 2, -size / 2, -size / 2);
	glTexCoord2f(1, 1);
	glVertex3f(size / 2, -size / 2, -size / 2);
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 100, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, "right.bmp");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 100, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, "left.bmp");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 100, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, "top.bmp");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 100, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, "bottom.bmp");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 100, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, "back.bmp");
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 100, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, "front.bmp");

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}
	
	/* initialise the correct version of openGL */
	instantiateVersion();


	// Fehler werden auf stderr ausgegeben, s. o.
	glfwSetErrorCallback(error_callback);

	// Open a window and create its OpenGL context
	// glfwWindowHint vorher aufrufen, um erforderliche Resourcen festzulegen
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Projekt: Taide", NULL, NULL);
	
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; 
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	glfwSetCursorPosCallback(window, curserPosionCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorEnterCallback(window, curserEnterCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/* Is used for the Skybox */

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);



	glDepthMask(GL_FALSE);
	glUseProgram(skyboxID);
	vector<std::string> faces = {"right.jpg","left.jpg","top.jpg","bottom.jpg","back.jpg","front.jpg"};
	unsigned int cubemapTexture = loadCubemap(faces);


	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	// ... draw rest of the scene

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Auf Keyboard-Events reagieren
	glfwSetKeyCallback(window, key_callback);

	// Dark blue background
	glClearColor(0.45f, 0.49f, 0.51f, 0.5f);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	skyboxID = LoadShaders("skybox.vertexshader", "skybox.fragmentshader");

	// Shader auch benutzen !
	glUseProgram(programID);

	obj *houseObj = new obj("house.obj");
	obj *doorObj = new obj("door.obj");
	//Load the texture
	GLuint TextureHouse = loadBMP_custom("bricks.bmp");
	GLuint TextureDoor = loadBMP_custom("door.bmp");


	// Eventloop
	while (!glfwWindowShouldClose(window))
	{
		/* finds out wheather the esc button is pressed */
		processInput(window);


		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//draw Skybox
		drawSkybox(50.0);


		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		Projection = glm::perspective(90.0f, 4.0f / 3.0f, 0.1f, 100.0f);

		// Camera matrix
		// View ist die Camera
		View = glm::lookAt(cameraPos, cameraPos + cameraFront + (neigungXVec * neigungX), cameraUp);

		
	    //Dreht das Haus
		Model = glm::mat4(1.0f);
		Model = glm::rotate(Model, angleX, glm::vec3(1, 0, 0));
		Model = glm::rotate(Model, angleY, glm::vec3(0, 1, 0));
		Model = glm::rotate(Model, angleZ, glm::vec3(0, 0, 1));
		//drawHouse();
		openDoor();

		glm::mat4 Save = Model;


		/* Ermöglichung der Rotation mit den Tasten 1,2,3,4 */

		//Model = glm::rotate(Model, angleZSeq1, glm::vec3(0, 0, 1));
		//Model = glm::rotate(Model, angleYSeq1, glm::vec3(0, 0, 1));

		//drawSeq(0.5);


		//Model = glm::translate(Model, glm::vec3(0, 0.5, 0));
		//Model = glm::rotate(Model, angleZSeq2, glm::vec3(0, 0, 1));
		//drawSeq(0.4);


		//Model = glm::translate(Model, glm::vec3(0, 0.4, 0));
		//Model = glm::rotate(Model, angleZSeq3, glm::vec3(0, 0, 1));
		//drawSeq(0.3);

		glm::vec4 lightPos = Model * glm::vec4(0, 0.3, 0, 1);
		glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
		//glUniform3f(glGetUniformLocation(skyboxID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

		//Model = Save;
		//Model = glm::translate(Model, glm::vec3(0.5, 0.0, 0.0)); //aenderung der Position des Objektes
		//Model = glm::scale(Model, glm::vec3(1.0 / 1000.0, 1.0 / 1000.0, 1.0 / 1000.0));



		sendMVP();

		Model = glm::mat4(1.0f);
		Model = glm::rotate(Model, angleX, glm::vec3(1, 0, 0));
		Model = glm::rotate(Model, angleY, glm::vec3(0, 1, 0));
		Model = glm::rotate(Model, angleZ, glm::vec3(0, 0, 1));

		glBindTexture(GL_TEXTURE_2D, TextureHouse);

		houseObj->draw();
		glBindTexture(GL_TEXTURE_2D, TextureDoor);

		doorObj->draw();



		//Model = glm::translate(Model, glm::vec3(1.5, 0.0, 0.0));


		//Draw Sphere in center
		/*
		Model = Save;
		Model = glm::scale(Model, glm::vec3(0.5, 0.5, 0.5));
		sendMVP();
		drawSphere(10, 10);
		*/

		// Draw Cube right Position
		/*
		Model = Save;
		Model = glm::translate(Model, glm::vec3(-1.5, 0.0, 0.0));
		Model = glm::scale(Model, glm::vec3(0.5, 0.5, 0.5));
		sendMVP(); //
		drawCube();
		*/

		glBindVertexArray(skyboxVAO);


		// Swap buffers
		glfwSwapBuffers(window);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Poll for and process events 
		glfwPollEvents();
		do_movement();

		/* Licht */
		//glm::vec3 lightPos = glm::vec3(4, 4, -4);
	}

	// Cleanup VBO and shader
	//glDeleteBuffers(1, &vertexbuffer);

	glDeleteTextures(1, &TextureHouse);
	glDeleteTextures(1, &TextureDoor);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);


	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	delete (houseObj);
	delete (doorObj);
	killSkyBox();
	return 0;
}

