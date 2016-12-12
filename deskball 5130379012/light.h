#pragma once
#include <GL/glut.h>

struct Light
{
	bool open = true;

	int lightnum;
	GLfloat position[4];
	GLfloat globalAmbient[4];
	GLfloat diffuse[4];
	GLfloat ambient[4];
	GLfloat specular[4];

	bool spot;
	GLfloat spotDirection[3];
	float spotExponent;
	float spotCutoff;
};