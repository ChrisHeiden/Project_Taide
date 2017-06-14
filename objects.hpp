#ifndef OBJECTS_HPP
#define OBJECTS_HPP

void drawWireCube(); // Wuerfel mit Kantenlaenge 2 im Drahtmodell
void drawCube();     // Bunter Wuerfel mit Kantenlaenge 2
void drawSphere(GLuint slices, GLuint stacks); // Kugel mit radius 1 bzw. Durchmesser 2
void drawHouse2();
void drawRoof();
void drawDoor();

#endif


/*
-15.0f,-1.0f,0.0f, -15.0f,-1.0f,-15.0f, 7.5f,4.0f,-7.5f,
15.0f,-1.0f,0.0f, 15.0f,-1.0f,-15.0f, 7.5f,4.0f,-7.5f,
-15.0f,-1.0f,0.0f, 15.0f,-1.0f,0.0f, 7.5f,4.0f,-7.5f,
-15.0f,-1.0f,-15.0f, 15.0f,-1.0f,-15.0f, 7.5f,4.0f,-7.5f,
*/