#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include "objects.hpp"
#include "objloader.hpp"
#include "texture.hpp"
#include "obj3d.h"

#include <iostream>

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
		neigungX += 0.1;
	}
	if(keys[GLFW_KEY_DOWN])
	{
		neigungX -= 0.1;
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
		mat4 ViewTemp(1.0f);
		ViewTemp = glm::rotate(ViewTemp, angleX, glm::vec3(1, 0, 0));
		View *= ViewTemp;

	}
	if (keys[GLFW_KEY_DOWN])
	{

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

void instantiateVersion()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

/* DRAW THE OUTSIDE OF THE GALERY IN THE SCENE */
void drawHouse() {
	glm::mat4 Save = Model;

	/* Draw the door*/
	Model = glm::scale(Model, glm::vec3(0.5, 1.1, 0.05)); 
	sendMVP();
	drawCube();
	Model = Save;

	if (initialised == false) {
		initialised = true;
		Door = Model;

		for (int i = 0; i < 16; ++i) {
			//cout << Door[i] << endl;
		}
	}
	
	/* Draw front - right*/
	Model = glm::scale(Model, glm::vec3(2.5, 1, 0.05));
	Model = glm::translate(Model, glm::vec3(1.2, 0, 0));
	sendMVP();
	drawCube();
	Model = Save;
	
	/* Draw front - left*/
	Model = glm::scale(Model, glm::vec3(2.5, 1, 0.05));
	Model = glm::translate(Model, glm::vec3(-1.2, 0, 0));
	sendMVP();
	drawCube();
	Model = Save;

	/* Draw side - right*/
	Model = glm::scale(Model, glm::vec3(0.05, 1, 25));
	Model = glm::translate(Model, glm::vec3(110, 0, 1));
	sendMVP();
	drawCube();
	Model = Save;

	/* Draw side - left*/
	Model = glm::scale(Model, glm::vec3(0.05, 1, 25));
	Model = glm::translate(Model, glm::vec3(-110, 0, 1));
	sendMVP();
	drawCube();
	Model = Save;


	/* Draw back - right*/
	Model = glm::scale(Model, glm::vec3(3, 1, 0.05));
	Model = glm::translate(Model, glm::vec3(0.95, 0, 1000));
	sendMVP();
	drawCube();
	Model = Save;

	/* Draw back - left*/
	Model = glm::scale(Model, glm::vec3(3, 1, 0.05));
	Model = glm::translate(Model, glm::vec3(-0.95, 0, 1000));
	sendMVP();
	drawCube();
	Model = Save;
	
}

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
int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}
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

	// Auf Keyboard-Events reagieren
	glfwSetKeyCallback(window, key_callback);

	// Dark blue background
	glClearColor(0.45f, 0.49f, 0.51f, 0.5f);

	// Create and compile our GLSL program from the shaders
	//programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader"); //Shader geändert zur Bestimmung einer anderen Farbe
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Shader auch benutzen !
	glUseProgram(programID);

	//Obj3D *ob = new Obj3D("teapot.obj");
	//Load the texture
	GLuint Texture = loadBMP_custom("door.bmp");

	// Eventloop
	while (!glfwWindowShouldClose(window))
	{

		//FUNBktionen aufrufen callback [mouse....]

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);

		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		Projection = glm::perspective(90.0f, 4.0f / 3.0f, 0.1f, 100.0f);

		// Camera matrix
		// View ist die Camera

		View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


		/* if(camera neigung größer als 45, keine Bewegung mehr) */ //<----------------------------------------- Prüfen, ob es größer als 45 grad ist
		mat4 ViewTemp(1.0f);

		while (neigungX >= 25) 
		{
			neigungX = 45;
		}

		ViewTemp = glm::rotate(ViewTemp, neigungX, glm::vec3(1, 0, 0));
		//ViewTemp = glm::rotate(ViewTemp, neigungX, glm::vec3(1, 0, 0));
		View *= ViewTemp;
		
		/* Var, die die temporären Daten speichert */
		
	    //Dreht das Haus
		Model = glm::mat4(1.0f);
		Model = glm::rotate(Model, angleX, glm::vec3(1, 0, 0));
		Model = glm::rotate(Model, angleY, glm::vec3(0, 1, 0));
		Model = glm::rotate(Model, angleZ, glm::vec3(0, 0, 1));
		drawHouse();
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

		//glm::vec4 lightPos = Model * glm::vec4(0, 0.3, 0, 1);
		//glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);

		//Model = Save;
		//Model = glm::translate(Model, glm::vec3(0.5, 0.0, 0.0)); //aenderung der Position des Objektes
		//Model = glm::scale(Model, glm::vec3(1.0 / 1000.0, 1.0 / 1000.0, 1.0 / 1000.0));



		sendMVP();
		//ob->draw();




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

		// Swap buffers
		glfwSwapBuffers(window);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Poll for and process events 
		glfwPollEvents();
		do_movement();

		/* Licht */
		glm::vec3 lightPos = glm::vec3(4, 4, -4);
	}

	// Cleanup VBO and shader
	//glDeleteBuffers(1, &vertexbuffer);

	glDeleteTextures(1, &Texture);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	//delete (ob);
	return 0;
}

