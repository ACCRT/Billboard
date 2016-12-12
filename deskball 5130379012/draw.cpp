#include "draw.h"

#include <GL/glut.h>
#include <windowsx.h>
#include <Windows.h>
#include "terrain.h"
#include "vboindex.h"
#include <iostream>
using namespace std;

float angle = 0;

extern GLuint id;
extern GLuint perlin_id;
extern GLuint perlin_id_3d;

extern std::vector<unsigned int> indices;
extern std::vector<glm::vec3> indexed_vertices;
extern std::vector<glm::vec2> indexed_uvs;
extern std::vector<glm::vec2> indexed_uvhs;
extern std::vector<glm::vec3> indexed_normals;

extern GLuint vertexbuffer;
extern GLuint uvhbuffer;
extern GLuint uvbuffer;
extern GLuint normalbuffer;
extern GLuint elementbuffer;

extern vector<vector<float>> camera;

#define PI 3.1415926

void glutWireRec(float length, float height, float width)
{
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(length, 0, 0);
	glVertex3f(length, height, 0);
	glVertex3f(0, height, 0);

	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, width);
	glVertex3f(0, height, width);
	glVertex3f(0, height, 0);

	glVertex3f(length, 0, 0);
	glVertex3f(length, 0, width);
	glVertex3f(length, height, width);
	glVertex3f(length, height, 0);

	glVertex3f(length, 0, width);
	glVertex3f(0, 0, width);
	glVertex3f(0, height, width);
	glVertex3f(length, height, width);

	glVertex3f(0, height, 0);
	glVertex3f(length, height, 0);
	glVertex3f(length, height, width);
	glVertex3f(0, height, width);

	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, width);
	glVertex3f(length, 0, width);
	glVertex3f(length, 0, 0);
	glEnd();
}

void glutSolidRec(float length, float height, float width)
{
	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, perlin_id);
	glEnable(GL_NORMALIZE);
	glNormal3f(0, 0, 1);
	/*for (int j = 0;j < height;j++)
		for (int i = 0;i < length;i++)
		{
			glBegin(GL_POLYGON);
			glVertex3f(i, 0, 0);
			glEnd();
		}*/
	glBegin(GL_POLYGON);
	glNormal3f(0,0,-1);
	for (int i = 0;i < length;i++)
	{
		glTexCoord2f(i / length, 0);
		glVertex3f(i, 0, 0);
	}
	/*glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0);
	glVertex3f(length, 0, 0);*/
	for (int i = 0;i < height;i++)
	{
		glTexCoord2f(1, i/height);
		glVertex3f(length, i, 0);
	}
	for (int i = 0;i < length;i++)
	{
		glTexCoord2f(1-i/length, 1);
		glVertex3f(length-i, height, 0);
	}
	for (int i = 0;i < height;i++)
	{
		glTexCoord2f(0, 1-i/height);
		glVertex3f(0, height-i, 0);
	}

	/*glTexCoord2f(1, 1);
	glVertex3f(length, height, 0);
	glTexCoord2f(0, 1);
	glVertex3f(0, height, 0);*/
	/*
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, width);
	glVertex3f(0, height, width);
	glVertex3f(0, height, 0);

	glVertex3f(length, 0, 0);
	glVertex3f(length, 0, width);
	glVertex3f(length, height, width);
	glVertex3f(length, height, 0);

	glVertex3f(length, 0, width);
	glVertex3f(0, 0, width);
	glVertex3f(0, height, width);
	glVertex3f(length, height, width);

	glVertex3f(0, height, 0);
	glVertex3f(length, height, 0);
	glVertex3f(length, height, width);
	glVertex3f(0, height, width);

	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, width);
	glVertex3f(length, 0, width);
	glVertex3f(length, 0, 0);*/
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
}

void glutWireSphere(ball &b)
{
	glPushMatrix();
	glTranslated(b.p.x, b.p.y, b.p.z);
	glRotatef(-angle, 0.0f, 1.0f, 0.0f);
	glutWireSphere(b.radius, 10, 10);
	glPopMatrix();
}

void glutSolidSphere(ball &b)
{
	glBindTexture(GL_TEXTURE_2D, perlin_id);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);						// 自动生成s方向纹理坐标
	glEnable(GL_TEXTURE_GEN_T);						// 自动生成t方向纹理坐标

	glPushMatrix();
	double rx = (b.p.z+100) * cos(int(b.p.x) / 1080.0 * 2 * PI)*sin(int(b.p.y) / 1080.0 * PI);
	double ry = (b.p.z+100) * sin(int(b.p.x) / 1080.0 * 2 * PI)*sin(int(b.p.y) / 1080.0 * PI);
	double rz = (b.p.z+100) * cos(int(b.p.y) / 1080.0  * PI);
	
	glTranslated(rx,ry,rz);
	//cout << b.p.z << endl;
	//glTranslated(b.p.x, b.p.y, b.p.z);
	glRotatef(-angle, 0.0f, 1.0f, 0.0f);

	glutSolidSphere(b.radius, 10, 10);

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);						// 禁止自动生成纹理坐标
	glDisable(GL_TEXTURE_GEN_T);
}

void glutSolidFlag(flag &f)
{
	glPushMatrix();
	glTranslatef(0, 0,400);
	
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	gluCylinder(quadratic, f.p.radius, f.p.radius, f.p.length, 32, 32);
	gluDeleteQuadric(quadratic);

	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);
	// 双面渲染
	glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT, GL_FILL);
	//glPolygonMode(GL_BACK, GL_FILL);
	
	glBindTexture(GL_TEXTURE_2D, perlin_id);
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 5, 0, 1, 5 * 3, 3, &f.control_point[0][0][0]);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &f.uv_point[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);

	glDisable(GL_MAP2_VERTEX_3);
	glDisable(GL_MAP2_TEXTURE_COORD_2);
	glDisable(GL_NORMALIZE);
	glDisable(GL_TEXTURE_2D);

	//glEnable(GL_CULL_FACE);

	glPopMatrix();
}

int DrawGLScene(particleEngine &pe)	// 绘制粒子
{
	glBindTexture(GL_TEXTURE_2D, pe.vertex_id);
	glEnable(GL_TEXTURE_2D);
	//glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
	glDisable(GL_LIGHTING);

	glEnable(GL_BLEND);									// Enable Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Type Of Blending To Perform

	for (int loop = 0;loop<MAX_PARTICLES;loop++)					// 循环所有的粒子
	{
		if (pe.particle[loop].active)					// 如果粒子为激活的
		{
			float x = pe.particle[loop].x;				// 返回X轴的位置
			float y = pe.particle[loop].y;				// 返回Y轴的位置
			float z = pe.particle[loop].z;				// 返回Z轴的位置
			
			float rx = (z+100 )*cos(x / 1080.0 * 2 * PI)*sin(y / 1080.0* PI);
			float ry = (z+100 )*sin(x / 1080.0 * 2 * PI)*sin(y / 1080.0 * PI);
			float rz = (z+100 )*cos(y / 1080.0  * PI);

			float radius = 400 * cos(PI / 4)*2;
			//cout << radius << endl;
			//if (rx<radius && rx>-radius && rz<radius && rz>-radius && ry<radius && ry>-radius)
			//{
			//	if((camera[0][0]*rx+camera[0][1]*ry+camera[0][2]*rz)<0)
			//		continue;
			//}

			glPushMatrix();
			glTranslatef((z+100)*cos(x / 1080.0 * 2 * PI)*sin(y / 1080.0* PI), (z+100)*sin(x / 1080.0 * 2 * PI)*sin(y / 1080.0  * PI), (z+100)*cos(y  / 1080.0  * PI));


			// 设置粒子颜色
			glColor4f(pe.particle[loop].r/255.0f, pe.particle[loop].g/255.0f, pe.particle[loop].b/255.0f, (pe.particle[loop].life));
			glBegin(GL_TRIANGLE_STRIP);				// 绘制三角形带
			glTexCoord2d(1, 1); glVertex3f( 5.0f, 5.0f, 0);
			glTexCoord2d(0, 1); glVertex3f(- 5.0f,  5.0f, 0);
			glTexCoord2d(1, 0); glVertex3f( 5.0f,  - 5.0f, 0);
			glTexCoord2d(0, 0); glVertex3f(- 5.0f, - 5.0f, 0);
			glEnd();
			glBegin(GL_TRIANGLE_STRIP);				// 绘制三角形带
			glTexCoord2d(1, 1); glVertex3f(0, 5.0f, 5.0f);
			glTexCoord2d(0, 1); glVertex3f(0, -5.0f, 5.0f);
			glTexCoord2d(1, 0); glVertex3f(0, 5.0f, -5.0f);
			glTexCoord2d(0, 0); glVertex3f(0, -5.0f, -5.0f);
			glEnd();
			glBegin(GL_TRIANGLE_STRIP);				// 绘制三角形带
			glTexCoord2d(1, 1); glVertex3f(5.0f, 0, 5.0f);
			glTexCoord2d(0, 1); glVertex3f(-5.0f, 0, 5.0f);
			glTexCoord2d(1, 0); glVertex3f(5.0f, 0, -5.0f);
			glTexCoord2d(0, 0); glVertex3f(-5.0f, 0, -5.0f);
			glEnd();
			glPopMatrix();
		}
	}

	glDepthMask(true);
	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	return TRUE;									// 绘制完毕成功返回
}

void DrawSkyBox(skybox & s)
{
	glDisable(GL_DEPTH_TEST);
	int fExtent = s.length;
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	//手动指定纹理坐标
	glBindTexture(GL_TEXTURE_2D, s.vertex_id[4]);
	glBegin(GL_QUADS);
	//-x
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);
	glEnd();

	//+x
	glBindTexture(GL_TEXTURE_2D, s.vertex_id[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(fExtent, fExtent, -fExtent);
	glEnd();

	//+y
	glBindTexture(GL_TEXTURE_2D, s.vertex_id[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, fExtent);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, fExtent);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(fExtent, fExtent, -fExtent);
	glEnd();

	//-y
	glBindTexture(GL_TEXTURE_2D, s.vertex_id[1]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);
	glEnd();


	//-z
	glBindTexture(GL_TEXTURE_2D, s.vertex_id[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-fExtent, -fExtent, -fExtent);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(fExtent, -fExtent, -fExtent);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(fExtent, fExtent, -fExtent);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-fExtent, fExtent, -fExtent);
	glEnd();

	//+z
	glBindTexture(GL_TEXTURE_2D, s.vertex_id[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-fExtent, fExtent, fExtent);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(fExtent, fExtent, fExtent);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(fExtent, -fExtent, fExtent);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-fExtent, -fExtent, fExtent);
	glEnd();
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
}

void DrawTerrain()
{
	/*int y, x;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, perlin_id);

	for (y = 0;y <= TERRAINWIDTH-2;y++)
	{
		glBegin(GL_TRIANGLE_STRIP);   
		for (x = 0;x <= TERRAINHEIGHT-1;x++)
		{
			glNormal3f(terrain[x][y].norx, terrain[x][y].nory, terrain[x][y].norz);
			glTexCoord2f(terrain[x][y].s, terrain[x][y].t);
			glVertex3f(terrain[x][y].x, terrain[x][y].y, terrain[x][y].z);
			glNormal3f(terrain[x][y + 1].norx, terrain[x][y + 1].nory, terrain[x][y + 1].norz);
			glTexCoord2f(terrain[x][y + 1].s, terrain[x][y + 1].t);
			glVertex3f(terrain[x][y + 1].x, terrain[x][y + 1].y, terrain[x][y + 1].z);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);*/
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glPushMatrix();
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_3D);
	//glDisable(GL_LIGHTING);
	glBindTexture(GL_TEXTURE_3D, perlin_id_3d);

	// to bind uvbuffer to GL_TEXTURE0
	glBindBuffer(GL_ARRAY_BUFFER, uvhbuffer);
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(3, GL_FLOAT, 0, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvhbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// 3rd attribute buffer : normals
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glNormalPointer(GL_FLOAT, 0, 0);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_TRUE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
		);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableClientState(GL_NORMAL_ARRAY);


	glBindBuffer(GL_ARRAY_BUFFER, uvhbuffer);
	glClientActiveTexture(GL_TEXTURE0);
	glActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	//glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_3D);
	//glEnable(GL_LIGHTING);
	glPopMatrix();
	/*
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, perlin_id);

	// to bind uvbuffer to GL_TEXTURE0
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	
	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
		);

	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(2);
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);*/
}