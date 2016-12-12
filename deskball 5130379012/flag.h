#pragma once

#include <GL/glew.h>

struct pole
{
	float length;
	float radius;
};

struct flag
{
	GLfloat control_point[3][5][3];
	//GLfloat speed[3][5][3];
	GLfloat uv_point[2][2][2] = { {{1,1},{0,1}}, {{1,0},{0,0}} };
	GLuint vertex_id;

	pole p;
};

