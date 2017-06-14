#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.hpp"
#include "objects.hpp"
#include "objloader.hpp"
#include "texture.hpp"

using namespace glm;
using namespace std;


class obj
{
public:
	obj(const char * name);
	~obj();

	void draw();

private:
	GLuint id;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	GLuint vertexbuffer;
	GLuint VertexArrayIDObj;
	GLuint normalbuffer; // Hier alles analog für Normalen in location == 2
	GLuint uvbuffer; // Hier alles analog für Texturkoordinaten in location == 1 (2 floats u und v!)

};

#endif
