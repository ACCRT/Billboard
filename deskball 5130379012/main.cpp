#include "textureLoader.h"
#include "flag.h"

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

#include <GL/glut.h>
#include <cmath>
#include "ball.h"
#include "draw.h"
#include "impact.h"
#include "particle.h"
#include <iostream>
#include <windowsx.h>
#include <Windows.h>
#include <time.h>

#include "controls.h"
#include "perlin.h"
#include "color.h"
#include "light.h"
#include "terrain.h"
#include "vboindex.h"
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <GL/glew.h>
#include <vector>

#define B1_NUM			6
#define B2_NUM			6

#define POLE_LENGTH		100

extern float angle;
int cycle = 0;
float pole_angle = 0;
int delay = 0;

flag f;

ball b_1[B1_NUM];
ball b_2[B2_NUM];
ball sp;
ball cue;

particleEngine particle[13];
Light light[2];

skybox sky;

bool collision_11[B1_NUM][B1_NUM];
bool collision_12[B1_NUM][B2_NUM];
bool collision_22[B2_NUM][B2_NUM];
bool collision_sp1[B1_NUM];
bool collision_sp2[B2_NUM];
bool collision_cue1[B1_NUM];
bool collision_cue2[B2_NUM];
bool collision_spcue;

std::vector<unsigned int> indices;
std::vector<glm::vec3> indexed_vertices;
std::vector<glm::vec3> indexed_uvhs;
std::vector<glm::vec2> indexed_uvs;
std::vector<glm::vec3> indexed_normals;

GLuint vertexbuffer;
GLuint uvbuffer;
GLuint uvhbuffer;
GLuint normalbuffer;
GLuint elementbuffer;

#define WINDOW_LENGTH	1024
#define WINDOW_HEIGHT	768
#define WINDOW_X		0
#define WINDOW_Y		0

#define TABLELENGTH 1080
#define TABLEWIDTH 1080

#define PI 3.1415926535

#define lerp(t, a, b) ( a + t * (b - a) )	// 线性函数

GLuint id;
GLuint perlin_id;
GLuint perlin_id_3d;

void init();
void init_ball();
void init_flag();
void init_particle();
void init_skybox();
void init_light();
void init_perlin();
void init_vboindex();
void init_perlin_3d();

void idle_ball();
void idle_flag();
void idle_particle();
void idle_light();
void idle_terrain();

void set_array(GLfloat* a, GLint* b, int num);
void set_array(GLfloat* a, GLfloat* b, int num);

extern GLint terrain_colors[14 * 6][3];
int terrain_color_num = 0;
GLfloat terrain_color[3];
int terrain_color_dely = 0;
bool terrain_color_foward = 1;

void error_detect()
{
	int a = glGetError();
	switch (a)
	{
	case GL_INVALID_ENUM:
		cout << ("GL Invalid Enum\n");
		break;
	case GL_INVALID_VALUE:
		cout << ("GL Invalid Value\n");
		break;
	case GL_INVALID_OPERATION:
		cout << ("GL Invalid Operation\n");
		break;
	case GL_OUT_OF_MEMORY:
		cout << ("GL Out Of Memory\n");
		break;
		//case GL_INVALID_FRAMEBUFFER_OPERATION:
		//	return ("GL Invalid FrameBuffer Operation\n");
	case  GL_STACK_OVERFLOW:
		cout << ("GL Stack Overflow\n");
		break;
	case GL_STACK_UNDERFLOW:
		cout << ("GL Stack Underflow\n");
		break;
		//case GL_TABLE_TOO_LARGE:
		//	return ("GL Table Too Large\n");
	};
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.0, 0.0);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluLookAt(100.0, 100.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void setColor(GLint *a)
{
	glColor3f(a[0]/255.0, a[1]/255.0, a[2]/255.0);
}
void setColor(GLfloat *a)
{
	glColor3f(a[0], a[1], a[2] );
}

void set_acceleration(ball &b)
{
	int terrianx = int(cue.p.x / (TABLELENGTH / TERRAINHEIGHT));
	int terriany = int(cue.p.y / (TABLEWIDTH / TERRAINWIDTH));
	glm::vec3 nor(
		terrain[terrianx][terriany].norx,
		terrain[terrianx][terriany].nory,
		terrain[terrianx][terriany].norz
		);
	glm::vec3 g(0, 0, -0.1);
	float nsize = glm::dot(nor, g);
	glm::vec3 tmp = nsize*nor;
	double f = 0.08*nsize;
	int vsize = sqrt(b.v.x*b.v.x + b.v.y*b.v.y);
	b.a.x = (g - tmp).x;
	b.a.y = (g - tmp).y;
	if (vsize != 0)
	{
		b.a.x -= b.v.x / vsize*f;
		b.a.y -= b.v.y / vsize*f;
	}
}

void lineSegment()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glColor3f(0.9, 0.6, 0.2);						

	glMatrixMode(GL_MODELVIEW);

	for (int i = 0;i < 2;i++)
	{
		glLightfv(light[i].lightnum, GL_POSITION, light[i].position);
		glLightfv(light[i].lightnum, GL_SPOT_DIRECTION, light[i].spotDirection);
		if (light[i].open) glEnable(light[i].lightnum);
	}
	//GLfloat color[4] = { 1,1,1,1 };
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glEnable(GL_LIGHTING);
	
	glColor3f(1, 1, 1);
	glDisable(GL_LIGHTING);
	DrawSkyBox(sky);
	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glPushMatrix();
	glColor3f(0.9, 0.6, 0.2);
	//glBlendFunc(GL_ONE, GL_ZERO);
	//glTranslated(-450,-300, 0);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, whiteColor);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, redColor);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, whiteColor);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat earth_mat_shininess = 128.0f;
	GLfloat earth_mat_emission[] = { 0.01f, 0.01f, 0.01f, 1.0f };
	glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, earth_mat_emission);

	// 设置镜面光反射属性及强镜面指数
	//glMaterialfv(GL_FRONT, GL_SPECULAR, 20.0f);
	glMateriali(GL_FRONT, GL_SHININESS, 5.0);


	//glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_AUTO_NORMAL);

	//glColor3f(1, 1, 1);
	//setColor(whiteColor);
	setColor(terrain_color);

	DrawTerrain();

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_AUTO_NORMAL);


	glMaterialfv(GL_FRONT, GL_AMBIENT, whiteColor);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteColor);

	earth_mat_shininess = 30.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);

	//glColor3f(1, 1, 1);
	//glutSolidRec(900, TABLEWIDTH, 0);

	//GLfloat earth_mat_ambient[] = { 0.0f, 0.0f, 0.5f, 1.0f };
	//GLfloat earth_mat_diffuse[] = { 0.0f, 0.0f, 0.5f, 1.0f };
	//GLfloat earth_mat_specular[] = { 0.0f,0.0f, 1.0f, 1.0f };
	//GLfloat earth_mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	//glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat_ambient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat_diffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat_specular);
	//glMaterialfv(GL_FRONT, GL_EMISSION, earth_mat_emission);
	earth_mat_shininess = 5.0f;
	glMaterialf(GL_FRONT, GL_SHININESS, earth_mat_shininess);

	for (int i = 0;i < 13;i++)
		DrawGLScene(particle[i]);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_AUTO_NORMAL);

	//glColor3f(0.9, 0.6, 0.2);
	setColor(green100Color);
	for (int i = 0;i < B1_NUM;i++)
		glutSolidSphere(b_1[i]);

	for (int i = 0;i < B2_NUM;i++)
		glutSolidSphere(b_2[i]);

	glColor3f(1, 1, 0.2);
	glutSolidSphere(sp);
	glColor3f(0, 0, 0);
	glutSolidSphere(cue);

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_AUTO_NORMAL);

	glColor3f(1, 1, 1);
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, whiteColor);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteColor);

	glutSolidFlag(f);
	
	if (cue.v.x == 0 && cue.v.y == 0)
	{
		glColor3f(0, 0, 0);
		//POINT pt;
		//GetCursorPos(&pt);
		//cout << pt.x << " " << pt.y << " " << cue.p.x << " " << cue.p.y << endl;
		//pole_angle = acos((pt.x - cue.p.x) / pow(pow(cue.p.x - pt.x, 2) + pow(cue.p.y - pt.y, 2), 0.5));
		glBegin(GL_LINES);
		glVertex3f(cue.p.x + 100 * cos(pole_angle), cue.p.y + 100 * sin(pole_angle), cue.p.z);
		glVertex3f(cue.p.x, cue.p.y, cue.p.z);
		glEnd();
	}

	glPopMatrix();
	glDisable(GL_LIGHTING);
	for (int i = 0;i < 2;i++)
		glDisable(light[i].lightnum);

	glFlush();
	//glutSwapBuffers();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void myidle(void)
{
	angle+= 1;
	if (angle>=360.0) angle = 0;

	idle_terrain();
	idle_flag();
	idle_ball();
	idle_particle();
	idle_light();
	computeMatricesFromInputs();
	lineSegment();
}

void idle_terrain()
{
	terrain_color_dely++;
	if (terrain_color_dely > 250)
	{
		if (terrain_color_foward)
			terrain_color_num++;
		else terrain_color_num--;
		terrain_color_dely=0;
	}

	if (terrain_color_num == 0)
		terrain_color_foward = 1;
	if (terrain_color_num == 83)
		terrain_color_foward = 0;

	if (terrain_color_foward)
		for (int i = 0;i < 3;i++)
			terrain_color[i] = lerp(terrain_color_dely / 250.0, (float)terrain_colors[terrain_color_num][i]/255.0, (float)terrain_colors[terrain_color_num + 1][i] / 255.0);
	else
		for (int i = 0;i < 3;i++)
			terrain_color[i] = lerp(terrain_color_dely / 250.0, (float)terrain_colors[terrain_color_num][i]/255.0, (float)terrain_colors[terrain_color_num - 1][i]/255.0);
}

void idle_flag()
{
	for (int i = 0;i < 3;i++)
	{
		for (int j = 0;j < 5;j++)
		{
			if (j == 0) f.control_point[i][j][0] = f.control_point[i][j][0];
			else if (i < 2)f.control_point[i][j][0] = 20 * sin(2 * (angle*3.14 / 180) - 4 * f.control_point[i][j][1]);
			else f.control_point[i][j][0] = 20 * cos(2 * (angle*3.14 / 180) - 4 * f.control_point[i][j][1]);
			f.control_point[i][j][1] = f.control_point[i][j][1];
			f.control_point[i][j][2] = f.control_point[i][j][2];
		}
	}
}

float set_z(ball &b)
{
	float tmpz = 0;
	/*for (int i = -1;i < 2;i++)
		for (int j = -1;j < 2;j++)
		{
			if (int((b.p.x + i) / (TABLELENGTH / TERRAINHEIGHT)) >0 && int((b.p.x + i) / (TABLELENGTH / TERRAINHEIGHT))<1080)
				if (int((b.p.y + j) / (TABLEWIDTH / TERRAINWIDTH))>0 && int((b.p.y + j) / (TABLEWIDTH / TERRAINWIDTH))<1080)
					if (tmpz < terrain[int((b.p.x + i) / (TABLELENGTH / TERRAINHEIGHT))][int((b.p.y + j) / (TABLEWIDTH / TERRAINWIDTH))].z)
						tmpz = terrain[int((b.p.x + i) / (TABLELENGTH / TERRAINHEIGHT))][int((b.p.y + j) / (TABLEWIDTH / TERRAINWIDTH))].z;
		}*/

	//tmpz = terrain[int((b.p.x))][int((b.p.y))].z;
	tmpz = 400;
	return tmpz;
}

void idle_ball()
{
	for (int i = 0;i < B1_NUM;i++)
	{
		for (int j = i + 1;j < B1_NUM;j++)
			if (impact_detect(b_1[i], b_1[j]))
			{
				if (collision_11[i][j] == 0)
				{
					collision_11[i][j] = 1;
					change_speed(b_1[i], b_1[j]);
				}
			}
			else collision_11[i][j] = 0;
			for (int j = 0;j < B2_NUM;j++)
				if (impact_detect(b_1[i], b_2[j]))
				{
					if (collision_12[i][j] == 0)
					{
						collision_12[i][j] = 1;
						change_speed(b_1[i], b_2[j]);
					}
				}
				else collision_12[i][j] = 0;
				if (impact_detect(b_1[i], sp))
				{
					if (collision_sp1[i] == 0)
					{
						collision_sp1[i] = 1;
						change_speed(b_1[i], sp);
					}
					else collision_sp1[i] = 0;
				}
				if (impact_detect(b_1[i], cue))
				{
					if (collision_cue1[i] == 0)
					{
						collision_cue1[i] = 1;
						change_speed(b_1[i], cue);
					}
					else collision_cue1[i] = 0;
				}
	}
	for (int i = 0;i < B2_NUM;i++)
	{
		for (int j = i + 1;j < B2_NUM;j++)
			if (impact_detect(b_2[i], b_2[j]))
			{
				if (collision_22[i][j] == 0)
				{
					collision_22[i][j] = 1;
					change_speed(b_2[i], b_2[j]);
				}
			}
			else collision_22[i][j] = 0;
			if (impact_detect(b_2[i], sp))
			{
				if (collision_sp2[i] == 0)
				{
					collision_sp2[i] = 1;
					change_speed(b_2[i], sp);
				}
				else collision_sp2[i] = 0;
			}
			if (impact_detect(b_2[i], cue))
			{
				if (collision_cue2[i] == 0)
				{
					collision_cue2[i] = 1;
					change_speed(b_2[i], cue);
				}
				else collision_cue2[i] = 0;
			}
	}

	if (impact_detect(cue, sp))
	{
		if (collision_spcue == 0)
		{
			collision_spcue = 1;
			change_speed(cue, sp);
		}
		else collision_spcue = 0;
	}

	for (int i = 0;i < B1_NUM;i++)
	{
		b_1[i].p.x += b_1[i].v.x;
		b_1[i].p.y += b_1[i].v.y;
		b_1[i].p.z = b_1[i].radius+set_z(b_1[i]);
		//b_1[i].v.x += b_1[i].a.x;
		//b_1[i].v.y += b_1[i].a.y;
		//b_1[i].v.x *= 0.999;
		//b_1[i].v.y *= 0.999;
		//set_acceleration(b_1[i]);
	}
	for (int i = 0;i < B2_NUM;i++)
	{
		b_2[i].p.x += b_2[i].v.x;
		b_2[i].p.y += b_2[i].v.y;
		b_2[i].p.z = b_2[i].radius +set_z(b_2[i]);
		//b_2[i].v.x += b_2[i].a.x;
		//b_2[i].v.y += b_2[i].a.y;
		//b_2[i].v.x *= 0.999;
		//b_2[i].v.y *= 0.999;
		//set_acceleration(b_2[i]);
	}

	sp.p.x += sp.v.x;
	sp.p.y += sp.v.y;
	//cout << sp.p.z <<" "<<sp.p.x<<" "<<sp.p.y<<endl;
	//sp.p.z += sp.v.z;
	//sp.p.z += terrain[int(sp.p.x/2)][int(sp.p.y/2)].z;
	//sp.v.x += sp.a.x;
	//sp.v.y += sp.a.y;

	cue.p.x += cue.v.x;
	cue.p.y += cue.v.y;
	cue.p.z = cue.radius + set_z(cue);
	//cue.v.x += cue.a.x;
	//cue.v.y += cue.a.y;
	//cue.v.x *= 0.999;
	//cue.v.y *= 0.999;
	//set_acceleration(cue);

	detect_reverse(sp);
	detect_reverse(cue);
	for (int i = 0;i < B1_NUM;i++)
	{
		detect_reverse(b_1[i]);
	}
	for (int i = 0;i < B2_NUM;i++)
	{
		detect_reverse(b_2[i]);
	}

	cycle++;
	if (cycle > 100 && cycle <= 200 && sp.p.z > terrain[(int)sp.p.x / (TABLELENGTH / TERRAINHEIGHT)][(int)sp.p.y / (TABLEWIDTH / TERRAINWIDTH)].z + sp.radius-100)
		sp.p.z -= 100 / 20;
	if (cycle > 100 && cycle <= 200 && sp.p.z <= terrain[(int)sp.p.x / (TABLELENGTH / TERRAINHEIGHT)][(int)sp.p.y / (TABLEWIDTH / TERRAINWIDTH)].z + sp.radius-100)
		sp.p.z = terrain[(int)sp.p.x / (TABLELENGTH / TERRAINHEIGHT)][(int)sp.p.y / (TABLEWIDTH / TERRAINWIDTH)].z + sp.radius-100;
	if (cycle > 200 && cycle < 300 && sp.p.z < 510)
		sp.p.z += 100 / 20;
	//sp.p.z = terrain[(int)sp.p.x / (TABLELENGTH / TERRAINHEIGHT)][(int)sp.p.y / (TABLEWIDTH / TERRAINWIDTH)].z + sp.radius;
	if (cycle == 300)
		cycle = 0;
}

void set_particle_active(particleEngine &pe, bool b)
{
	for (int loop = 0;loop < MAX_PARTICLES;loop++)					// 循环所有的粒子
		pe.particle[loop].active = b;
}

void idle_particle_ball(particleEngine &pe, ball &b,bool produce)
{
	if (pe.rainbow && (delay>25))	//变换颜色
		pe.col++;

	if (pe.col > 11)
	{
		pe.col = 0;
	}

	for (int loop = 0;loop < MAX_PARTICLES;loop++)					// 循环所有的粒子
	{
		if (pe.particle[loop].active)					// 如果粒子为激活的
		{
			pe.particle[loop].x += pe.particle[loop].xi / (pe.slowdown * 1000);	// 更新X坐标的位置
			pe.particle[loop].y += pe.particle[loop].yi / (pe.slowdown * 1000);	// 更新Y坐标的位置
			pe.particle[loop].z += pe.particle[loop].zi / (pe.slowdown * 1000);	// 更新Z坐标的位置

			pe.particle[loop].xi += pe.particle[loop].xg;			// 更新X轴方向速度大小
			pe.particle[loop].yi += pe.particle[loop].yg;			// 更新Y轴方向速度大小
			pe.particle[loop].zi += pe.particle[loop].zg;			// 更新Z轴方向速度大小

			pe.particle[loop].life -= pe.particle[loop].fade;		// 减少粒子的生命值

			if (pe.particle[loop].life < 0.0f && produce)					// 如果粒子生命值小于0
			{
				pe.particle[loop].life = 1.0f;				// 产生一个新的粒子
				pe.particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;	// 随机生成衰减速率

				pe.particle[loop].x = b.p.x;					// 新粒子出现在屏幕的中央
				pe.particle[loop].y = b.p.y;
				pe.particle[loop].z = b.p.z - b.radius + rand() % 10+10;

				pe.particle[loop].xi = pe.xspeed + float((rand() % 260) - 32.0f) * 10;	// 随机生成粒子速度
				pe.particle[loop].yi = pe.yspeed + float((rand() % 260) - 30.0f) * 10;
				pe.particle[loop].zi = 0;

				pe.particle[loop].r = pe.colors[pe.col][0];			// 设置粒子颜色
				pe.particle[loop].g = pe.colors[pe.col][1];
				pe.particle[loop].b = pe.colors[pe.col][2];
			}
		}
	}
}

void idle_particle()
{
	delay++;
	for (int i = 0;i < 6;i++)
		idle_particle_ball(particle[i], b_1[i],1);
	for (int i = 0;i < 6;i++)
		idle_particle_ball(particle[i + 6], b_2[i],1);

	if (abs(sp.p.z+100-sp.radius - terrain[(int)sp.p.x / (TABLELENGTH / TERRAINHEIGHT)][(int)sp.p.y / (TABLEWIDTH / TERRAINWIDTH)].z) < 0.1)
		idle_particle_ball(particle[12], sp, 1);
	else 
		idle_particle_ball(particle[12], sp, 0);
	if (delay > 25) delay = 0;
}

void idle_light()
{
	GLfloat light2PosType[] = { 800.0f*cos(cue.p.x / 1080.0 * 2 * PI)*sin(cue.p.y / 1080.0 * PI),800.0f*sin(cue.p.x / 1080.0 * 2 * PI)*sin(cue.p.y / 1080.0  * PI), 800.0f*cos(cue.p.y / 1080.0 *  PI),1.0 };
	GLfloat light2SpotDirection[] = { -cos(cue.p.x / 1080.0 * 2 * PI)*sin(cue.p.y / 1080.0 * PI),-sin(cue.p.x / 1080.0 * 2 * PI)*sin(cue.p.y / 1080.0  * PI),-cos(cue.p.y / 1080.0 *  PI) };
	set_array(light[1].position, light2PosType, 4);
	set_array(light[1].spotDirection, light2SpotDirection, 3);
	//glLightf(light[1].lightnum, GL_SPOT_EXPONENT, light[1].spotExponent); //聚光灯指数
	//glLightfv(light[1].lightnum, GL_SPOT_DIRECTION, light[1].spotDirection); //聚光灯方向
}

int main(int argc, char**argv)
{
	/*glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(WINDOW_X, WINDOW_Y);
	glutInitWindowSize(WINDOW_LENGTH, WINDOW_HEIGHT);
	glutCreateWindow("deskball");*/

	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "deskball", NULL, NULL);
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

	// Set the mouse at the center of the screen
	//glfwPollEvents();
	//glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	glewInit();

	init();
	for (int i = 0;i < B1_NUM;i++)
	{
		for (int j = 0;j < B1_NUM;j++)
			collision_11[i][j] = 0;
		for (int j = 0;j < B2_NUM;j++)
		collision_12[i][j] = 0;
	}
	for (int i = 0;i < B2_NUM;i++)
		for (int j = 0;j < B2_NUM;j++)
			collision_22[i][j] = 0;
	int repeat = 0;
	do {
		myidle();
		//glm::mat4 ProjectionMatrix = getProjectionMatrix();
		//glm::mat4 ViewMatrix = getViewMatrix();
		//glm::mat4 ModelMatrix = glm::mat4(1.0);
		//glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		error_detect();
		if (repeat == 30)
		{
			repeat = 0;
			handleKeys();
		}
		else repeat++;

		//cout << camera[0][0] << endl << camera[0][1] << endl << camera[0][2] << endl;
		//cout << camera[2][0] << endl << camera[2][1] << endl << camera[2][2] << endl << endl;
		
		//computeMatricesFromInputs();
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	/*glutDisplayFunc(lineSegment);
	glutIdleFunc(myidle);
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processNormalKeys);
	//glutReshapeFunc(reshape);
	glutMainLoop();*/

	return 0;
}

void init()
{
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f,0.0f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	//glOrtho(-WINDOW_LENGTH/2, WINDOW_LENGTH/2, -WINDOW_HEIGHT/2, WINDOW_HEIGHT/2, -2000.0, 2000.0);
	glFrustum(-48, 48, -36, 36, 70.0, 10000.0);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);

	//glPointSize(10);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(-600.0, -600.0, 600.0, 0.0, 0.0, 0.0, 1, 1, 1);
	LoadTexture("31963129.jpg", id);

	init_flag();
	init_skybox();
	init_light();
	init_perlin();
	init_perlin_3d();
	initTerrain();
	init_ball();
	init_particle();
	init_vboindex();

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// 设置s方向的纹理自动生成
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// 设置t方向的纹理自动生成
}

void init_ball()
{
	for (int i = 0;i < B1_NUM;i++)
	{
		b_1[i].radius = 10;
		b_1[i].p.x = 30 * i + 200;
		b_1[i].p.y = 30 * i + 200;
		b_1[i].p.z = b_1[i].radius+set_z(b_1[i]);
		b_1[i].v.x = 0.3*(i+1);
		b_1[i].v.y = 0.7*(i+1);
		b_1[i].v.z = 0;
		b_1[i].sp = 0;

		//set_acceleration(b_1[i]);
	}

	for (int i = 0;i < B2_NUM;i++)
	{
		b_2[i].radius = 10;
		b_2[i].p.x = 60 * i+60;
		b_2[i].p.y = 60 * i+15;
		b_2[i].p.z = b_2[i].radius+set_z(b_2[i]);
		b_2[i].v.x = 0.5*i;
		b_2[i].v.y = 0.5*i;
		b_2[i].v.z = 0;
		b_2[i].sp = 0;

		//set_acceleration(b_2[i]);

	}

	sp.p.x = 60;
	sp.p.y = 60;
	sp.p.z = 100+ sp.radius + set_z(sp);
	sp.radius = 10;
	sp.v.x = 2;
	sp.v.y = 4;
	sp.sp = 1;

	//set_acceleration(sp);

	cue.radius = 10;
	cue.p.x = 80;
	cue.p.y = 100;
	cue.p.z = cue.radius + set_z(cue);
	cue.sp = 0;

	//set_acceleration(cue);
}

void init_flag()
{
	f.p.radius = 5;
	f.p.length = 400;
	for (int i = 0;i < 3;i++)
	{
		for (int j = 0;j < 5;j++)
		{
			f.control_point[i][j][0] = 0;
			f.control_point[i][j][1] = 50*j;
			f.control_point[i][j][2] = f.p.length - 50 * i;
		}
	}

	LoadTexture("Untitled1.bmp", f.vertex_id);
}

void init_particle_ball(particleEngine &pe, ball &b)
{
	LoadTexture("Particle.bmp", pe.vertex_id);	// 载入粒子纹理
	int loop;
	for (loop = 0;loop < MAX_PARTICLES;loop++)				//初始化所有的粒子
	{
		pe.particle[loop].active = true;					// 使所有的粒子为激活状态
		pe.particle[loop].life = 1.0f;					// 所有的粒子生命值为最大
		pe.particle[loop].fade = float(rand() % 100) / 1000.0f + 0.003f;		// 随机生成衰减速率

		pe.col = pe.particlenumber;
		pe.particle[loop].r = pe.colors[loop*(12 / MAX_PARTICLES)][0];		// 粒子的红色颜色
		pe.particle[loop].g = pe.colors[loop*(12 / MAX_PARTICLES)][1];		// 粒子的绿色颜色
		pe.particle[loop].b = pe.colors[loop*(12 / MAX_PARTICLES)][2];		// 粒子的蓝色颜色

		pe.particle[loop].xi = float((rand() % 250) - 26.0f)*10.0f;		// 随机生成X轴方向速度
		pe.particle[loop].yi = float((rand() % 250) - 25.0f)*10.0f;		// 随机生成Y轴方向速度
		pe.particle[loop].zi = float((rand() % 250) - 25.0f)*10.0f;		// 随机生成Z轴方向速度

		pe.particle[loop].xg = 0.0f;						// 设置X轴方向加速度为0
		pe.particle[loop].yg = -0.8f;						//  设置Y轴方向加速度为-0.8
		pe.particle[loop].zg = 0.0f;						//  设置Z轴方向加速度为0

		pe.particle[loop].x = b.p.x;
		pe.particle[loop].y = b.p.y;
		pe.particle[loop].z = b.p.z-b.radius;
	}
}

void init_particle()
{
	/*for (int i = 0;i < 12;i++)
		for (int j = 0;j < 3;j++)
		{
			particle[i].colors[0][j] = blue50Color[j];
			particle[i].colors[1][j] = blue100Color[j];
			particle[i].colors[2][j] = blueA100Color[j];
			particle[i].colors[3][j] = blue200Color[j];
			particle[i].colors[4][j] = blueA200Color[j];
			particle[i].colors[5][j] = blue300Color[j];
			particle[i].colors[6][j] = blue400Color[j];
			particle[i].colors[7][j] = blueA400Color[j];
			particle[i].colors[8][j] = blue500Color[j];
			particle[i].colors[9][j] = blue600Color[j];
			particle[i].colors[10][j] = blue700Color[j];
			particle[i].colors[11][j] = blue800Color[j];
		}*/

	//glDisable(GL_DEPTH_TEST);						//禁止深度测试
	for (int i = 0;i < 6;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			particle[0].colors[0][j] = red50Color[j];
			particle[0].colors[1][j] = red100Color[j];
			particle[0].colors[2][j] = redA100Color[j];
			particle[0].colors[3][j] = red200Color[j];
			particle[0].colors[4][j] = redA200Color[j];
			particle[0].colors[5][j] = red300Color[j];
			particle[0].colors[6][j] = red400Color[j];
			particle[0].colors[7][j] = redA400Color[j];
			particle[0].colors[8][j] = red500Color[j];
			particle[0].colors[9][j] = red600Color[j];
			particle[0].colors[10][j] = red700Color[j];
			particle[0].colors[11][j] = red800Color[j];
		}

		for (int j = 0;j < 3;j++)
		{
			particle[1].colors[0][j] = purple50Color[j];
			particle[1].colors[1][j] = purple100Color[j];
			particle[1].colors[2][j] = purpleA100Color[j];
			particle[1].colors[3][j] = purple200Color[j];
			particle[1].colors[4][j] = purpleA200Color[j];
			particle[1].colors[5][j] = purple300Color[j];
			particle[1].colors[6][j] = purple400Color[j];
			particle[1].colors[7][j] = purpleA400Color[j];
			particle[1].colors[8][j] = purple500Color[j];
			particle[1].colors[9][j] = purple600Color[j];
			particle[1].colors[10][j] = purple700Color[j];
			particle[1].colors[11][j] = purple800Color[j];
		}

		for (int j = 0;j < 3;j++)
		{
			particle[2].colors[0][j] = indigo50Color[j];
			particle[2].colors[1][j] = indigoA100Color[j];
			particle[2].colors[2][j] = indigo200Color[j];
			particle[2].colors[3][j] = indigo300Color[j];
			particle[2].colors[4][j] = indigo400Color[j];
			particle[2].colors[5][j] = indigoA400Color[j];
			particle[2].colors[6][j] = indigo500Color[j];
			particle[2].colors[7][j] = indigo600Color[j];
			particle[2].colors[8][j] = indigo700Color[j];
			particle[2].colors[9][j] = indigoA700Color[j];
			particle[2].colors[10][j] = indigo800Color[j];
			particle[2].colors[11][j] = indigo900Color[j];
		}

		for (int j = 0;j < 3;j++)
		{
			particle[3].colors[0][j] = cyan50Color[j];
			particle[3].colors[1][j] = cyanA100Color[j];
			particle[3].colors[2][j] = cyan200Color[j];
			particle[3].colors[3][j] = cyan300Color[j];
			particle[3].colors[4][j] = cyan400Color[j];
			particle[3].colors[5][j] = cyanA400Color[j];
			particle[3].colors[6][j] = cyan500Color[j];
			particle[3].colors[7][j] = cyan600Color[j];
			particle[3].colors[8][j] = cyan700Color[j];
			particle[3].colors[9][j] = cyanA700Color[j];
			particle[3].colors[10][j] = cyan800Color[j];
			particle[3].colors[11][j] = cyan900Color[j];
		}

		for (int j = 0;j < 3;j++)
		{
			particle[4].colors[0][j] = teal50Color[j];
			particle[4].colors[1][j] = tealA100Color[j];
			particle[4].colors[2][j] = teal200Color[j];
			particle[4].colors[3][j] = teal300Color[j];
			particle[4].colors[4][j] = teal400Color[j];
			particle[4].colors[5][j] = tealA400Color[j];
			particle[4].colors[6][j] = teal500Color[j];
			particle[4].colors[7][j] = teal600Color[j];
			particle[4].colors[8][j] = teal700Color[j];
			particle[4].colors[9][j] = tealA700Color[j];
			particle[4].colors[10][j] = teal800Color[j];
			particle[4].colors[11][j] = teal900Color[j];
		}

		for (int j = 0;j < 3;j++)
		{
			particle[5].colors[0][j] = green50Color[j];
			particle[5].colors[1][j] = greenA100Color[j];
			particle[5].colors[2][j] = green200Color[j];
			particle[5].colors[3][j] = green300Color[j];
			particle[5].colors[4][j] = green400Color[j];
			particle[5].colors[5][j] = greenA400Color[j];
			particle[5].colors[6][j] = green500Color[j];
			particle[5].colors[7][j] = green600Color[j];
			particle[5].colors[8][j] = green700Color[j];
			particle[5].colors[9][j] = greenA700Color[j];
			particle[5].colors[10][j] = green800Color[j];
			particle[5].colors[11][j] = green900Color[j];
		}

		particle[i].particlenumber = i;
		init_particle_ball(particle[i], b_1[i]);
	}

	for (int i = 0;i < 6;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			particle[6].colors[0][j] = blue50Color[j];
			particle[6].colors[1][j] = blueA100Color[j];
			particle[6].colors[2][j] = blue200Color[j];
			particle[6].colors[3][j] = blue300Color[j];
			particle[6].colors[4][j] = blue400Color[j];
			particle[6].colors[5][j] = blueA400Color[j];
			particle[6].colors[6][j] = blue500Color[j];
			particle[6].colors[7][j] = blue600Color[j];
			particle[6].colors[8][j] = blue700Color[j];
			particle[6].colors[9][j] = blueA700Color[j];
			particle[6].colors[10][j] = blue800Color[j];
			particle[6].colors[11][j] = blue900Color[j];
			particle[6].particlenumber = i + 6;
		}
		for (int j = 0;j < 3;j++)
		{
			particle[7].colors[0][j] = orange50Color[j];
			particle[7].colors[1][j] = orangeA100Color[j];
			particle[7].colors[2][j] = orange200Color[j];
			particle[7].colors[3][j] = orange300Color[j];
			particle[7].colors[4][j] = orange400Color[j];
			particle[7].colors[5][j] = orangeA400Color[j];
			particle[7].colors[6][j] = orange500Color[j];
			particle[7].colors[7][j] = orange600Color[j];
			particle[7].colors[8][j] = orange700Color[j];
			particle[7].colors[9][j] = orangeA700Color[j];
			particle[7].colors[10][j] = orange800Color[j];
			particle[7].colors[11][j] = orange900Color[j];
		}
		for (int j = 0;j < 3;j++)
		{
			particle[8].colors[0][j] = pink50Color[j];
			particle[8].colors[1][j] = pinkA100Color[j];
			particle[8].colors[2][j] = pink200Color[j];
			particle[8].colors[3][j] = pink300Color[j];
			particle[8].colors[4][j] = pink400Color[j];
			particle[8].colors[5][j] = pinkA400Color[j];
			particle[8].colors[6][j] = pink500Color[j];
			particle[8].colors[7][j] = pink600Color[j];
			particle[8].colors[8][j] = pink700Color[j];
			particle[8].colors[9][j] = pinkA700Color[j];
			particle[8].colors[10][j] = pink800Color[j];
			particle[8].colors[11][j] = pink900Color[j];
		}
		for (int j = 0;j < 3;j++)
		{
			particle[9].colors[0][j] = lightBlue50Color[j];
			particle[9].colors[1][j] = lightBlueA100Color[j];
			particle[9].colors[2][j] = lightBlue200Color[j];
			particle[9].colors[3][j] = lightBlue300Color[j];
			particle[9].colors[4][j] = lightBlue400Color[j];
			particle[9].colors[5][j] = lightBlueA400Color[j];
			particle[9].colors[6][j] = lightBlue500Color[j];
			particle[9].colors[7][j] = lightBlue600Color[j];
			particle[9].colors[8][j] = lightBlue700Color[j];
			particle[9].colors[9][j] = lightBlueA700Color[j];
			particle[9].colors[10][j] = lightBlue800Color[j];
			particle[9].colors[11][j] = lightBlue900Color[j];
		}
		for (int j = 0;j < 3;j++)
		{
			particle[10].colors[0][j] = lime50Color[j];
			particle[10].colors[1][j] = limeA100Color[j];
			particle[10].colors[2][j] = lime200Color[j];
			particle[10].colors[3][j] = lime300Color[j];
			particle[10].colors[4][j] = lime400Color[j];
			particle[10].colors[5][j] = limeA400Color[j];
			particle[10].colors[6][j] = lime500Color[j];
			particle[10].colors[7][j] = lime600Color[j];
			particle[10].colors[8][j] = lime700Color[j];
			particle[10].colors[9][j] = limeA700Color[j];
			particle[10].colors[10][j] = lime800Color[j];
			particle[10].colors[11][j] = lime900Color[j];
		}
		for (int j = 0;j < 3;j++)
		{
			particle[11].colors[0][j] = amber50Color[j];
			particle[11].colors[1][j] = amberA100Color[j];
			particle[11].colors[2][j] = amber200Color[j];
			particle[11].colors[3][j] = amber300Color[j];
			particle[11].colors[4][j] = amber400Color[j];
			particle[11].colors[5][j] = amberA400Color[j];
			particle[11].colors[6][j] = amber500Color[j];
			particle[11].colors[7][j] = amber600Color[j];
			particle[11].colors[8][j] = amber700Color[j];
			particle[11].colors[9][j] = amberA700Color[j];
			particle[11].colors[10][j] = amber800Color[j];
			particle[11].colors[11][j] = amber900Color[j];
		}
		init_particle_ball(particle[i + 6], b_2[i]);
	}

	for (int i = 0;i < 3;i++)
	{
		particle[12].colors[0][i] = yellow50Color[i];
		particle[12].colors[1][i] = yellowA100Color[i];
		particle[12].colors[2][i] = yellow200Color[i];
		particle[12].colors[3][i] = yellow300Color[i];
		particle[12].colors[4][i] = yellow400Color[i];
		particle[12].colors[5][i] = yellowA400Color[i];
		particle[12].colors[6][i] = yellow500Color[i];
		particle[12].colors[7][i] = yellow600Color[i];
		particle[12].colors[8][i] = yellow700Color[i];
		particle[12].colors[9][i] = yellowA700Color[i];
		particle[12].colors[10][i] = yellow800Color[i];
		particle[12].colors[11][i] = yellow900Color[i];
	}
	particle[12].particlenumber = 12;
	init_particle_ball(particle[12], sp);
}

void set_array(GLfloat* a, GLfloat* b, int num)
{
	for (int i = 0;i < num;i++)
		a[i] = b[i];
}
void set_array(GLfloat* a, GLint* b, int num)
{
	for (int i = 0;i < num;i++)
		a[i] = b[i]/255.0f;
}

void init_light()
{
	//GL_LIGHT_MODEL_AMBIENT表示全局环境光线强度，由四个值组成。
	//GL_LIGHT_MODEL_LOCAL_VIEWER表示是否在近处观看，若是则设置为GL_TRUE，否则（即在无限远处观看）设置为GL_FALSE。
	//GL_LIGHT_MODEL_TWO_SIDE表示是否执行双面光照计算。如果设置为GL_TRUE，则OpenGL不仅将根据法线向量计算正面的光照，也会将法线向量反转并计算背面的光照。
	//GL_LIGHT_MODEL_COLOR_CONTROL表示颜色计算方式。

	//GL_AMBIENT表示各种光线照射到该材质上，经过很多次反射后最终遗留在环境中的光线强度（颜色）。
	//GL_DIFFUSE表示光线照射到该材质上，经过漫反射后形成的光线强度（颜色）。
	//GL_SPECULAR表示光线照射到该材质上，经过镜面反射后形成的光线强度（颜色）。
	//通常，GL_AMBIENT和GL_DIFFUSE都取相同的值，可以达到比较真实的效果。
	//使用GL_AMBIENT_AND_DIFFUSE可以同时设置GL_AMBIENT和GL_DIFFUSE属性。

	//GL_SHININESS属性。
	//该属性只有一个值，称为“镜面指数”，取值范围是0到128。
	//该值越小，表示材质越粗糙，点光源发射的光线照射到上面，也可以产生较大的亮点。该值越大，表示材质越类似于镜面，光源照射到上面后，产生较小的亮点。

	//GL_EMISSION属性。该属性由四个值组成，表示一种颜色。OpenGL认为该材质本身就微微的向外发射光线，以至于眼睛感觉到它有这样的颜色，但这光线又比较微弱，以至于不会影响到其它物体的颜色。

	GLfloat light1PosType[] = { -700.0,-700.0,700.0,0.0 };
	GLfloat ambient[] = { 0.5,0.5,0.5,1.0 };
	GLfloat globalAmbient[] = { 0.1,0.1,0.1,1.0 };

	light[0].lightnum = GL_LIGHT0;
	set_array(light[0].globalAmbient, globalAmbient,4);
	set_array(light[0].position, light1PosType, 4);
	
	set_array(light[0].ambient, globalAmbient, 4);
	set_array(light[0].diffuse, ambient,4 );
	set_array(light[0].specular, blackColor, 4);
	light[0].spotExponent = 0;

	GLfloat light2PosType[] = { 800.0f*cos(cue.p.x/1080.0*2*PI)*sin(cue.p.y / 1080.0 * PI),800.0f*sin(cue.p.x / 1080.0 * 2 * PI)*sin(cue.p.y / 1080.0  * PI), 800.0f*cos(cue.p.y / 1080.0 *  PI),1.0 };
	GLfloat light2SpotDirection[] = { -cos(cue.p.x / 1080.0 * 2 * PI)*sin(cue.p.y / 1080.0 * PI),-sin(cue.p.x / 1080.0 * 2 * PI)*sin(cue.p.y / 1080.0  * PI),-cos(cue.p.y / 1080.0 *  PI) };
	
	light[1].lightnum = GL_LIGHT1;
	light[1].spot = true;

	set_array(light[1].globalAmbient, globalAmbient, 4);
	set_array(light[1].position, light2PosType, 4);

	set_array(light[1].ambient,blackColor, 4);
	set_array(light[1].diffuse, redColor, 4);
	set_array(light[1].specular, whiteColor, 4);
	
	set_array(light[1].spotDirection, light2SpotDirection, 3);
	light[1].spotExponent = 30.0f;
	light[1].spotCutoff = 40.0f; // 180代表非聚光灯

	for (int i = 0;i < 2;i++)
	{
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light[i].globalAmbient);
		glLightfv(light[i].lightnum, GL_AMBIENT, light[i].ambient);
		glLightfv(light[i].lightnum, GL_DIFFUSE, light[i].diffuse);
		glLightfv(light[i].lightnum, GL_SPECULAR, light[i].specular);
		glLightfv(light[i].lightnum, GL_POSITION, light[i].position);

		if (light[i].spot)
		{
			glLightf(light[i].lightnum, GL_SPOT_CUTOFF, light[i].spotCutoff);  //光柱张角80度
			glLightf(light[i].lightnum, GL_SPOT_EXPONENT, light[i].spotExponent); //聚光灯指数
			glLightfv(light[i].lightnum, GL_SPOT_DIRECTION, light[i].spotDirection); //聚光灯方向
		}
	}
}

void init_skybox()
{
	LoadTexture("front.bmp", sky.vertex_id[0]);
	LoadTexture("back.bmp", sky.vertex_id[1]);
	LoadTexture("top.bmp", sky.vertex_id[2]);
	LoadTexture("bottom.bmp", sky.vertex_id[3]);
	LoadTexture("left.bmp", sky.vertex_id[4]);
	LoadTexture("right.bmp", sky.vertex_id[5]);
	sky.length =4000;
}

void init_perlin()
{
	BYTE bit[128 * 128 * 3];
	for (int i = 0;i < 128;i++)
		for (int j = 0;j < 128;j++)
			for (int z = 0;z < 3;z++)
			{
				srand((unsigned)time(NULL));
				double vec[2] = { 0.02*i,0.02*j };
				//double p = noise2(vec)*255;
				//double p = (noise2(vec) + 1) / 2;
				//p = (p * 10 - (int)(p * 10)) * 255;
				double p = (PerlinNoise2D(0.08*i, 0.08*j, 2, 2, 5)+1)/2*255;
				if (p == 255 / 2) cout << i << " " << j << endl;
				bit[i * 128 * 3 + j * 3 + z] = p;
				//cout << p << endl;
			}
	GenTexture2D(bit, perlin_id,128, 128);
}

void init_perlin_3d()
{
	BYTE* bit = new BYTE[80 * 80 * 80 * 3];
	srand((unsigned)time(NULL));
	for (int i = 0;i < 80;i++)
		for (int j = 0;j < 80;j++)
			for (int k = 0;k < 80;k++)
				{
					double vec[3] = {0.05*abs(40-i),0.05*abs(40-j),0.05*abs(40-k) };
					//double vec[3] = { 0.05*i,0.05*j,0.05*k };
					//double p = (noise3(vec) + 1) / 2 *255;
					double p = (noise3(vec)+1)/2 ;
					p = (p * 10 - (int)(p * 10)) * 255;
					if(p>255 || p <0) cout << p << endl;
					bit[i * 80* 80 * 3 + j * 80*3 + k*3] = p;
					bit[i * 80 * 80* 3 + j * 80 * 3 + k * 3+1] = p;
					bit[i * 80 * 80 * 3 + j * 80 * 3 + k * 3+2] = p;
				}
	GenTexture3D(bit, perlin_id_3d, 80, 80,80);
	delete [] bit;
}

void init_vboindex()
{
	/*
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	for (int y = 0;y < TERRAINWIDTH - 1;y++)
	{
		for (int x = 0;x < TERRAINHEIGHT - 1;x++)
		{
			normals.push_back(glm::vec3(terrain[x][y].rnorx, terrain[x][y].rnory, terrain[x][y].rnorz));
			uvs.push_back(glm::vec2(terrain[x][y].s, terrain[x][y].t));
			vertices.push_back(glm::vec3(terrain[x][y].rx, terrain[x][y].ry, terrain[x][y].rz));

			normals.push_back(glm::vec3(terrain[x][y + 1].rnorx, terrain[x][y + 1].rnory, terrain[x][y + 1].rnorz));
			uvs.push_back(glm::vec2(terrain[x][y + 1].s, terrain[x][y + 1].t));
			vertices.push_back(glm::vec3(terrain[x][y + 1].rx, terrain[x][y + 1].ry, terrain[x][y + 1].rz));

			normals.push_back(glm::vec3(terrain[x + 1][y].rnorx, terrain[x + 1][y].rnory, terrain[x + 1][y].rnorz));
			uvs.push_back(glm::vec2(terrain[x + 1][y].s, terrain[x + 1][y].t));
			vertices.push_back(glm::vec3(terrain[x + 1][y].rx, terrain[x + 1][y].ry, terrain[x + 1][y].rz));

			normals.push_back(glm::vec3(terrain[x+1][y+1].rnorx, terrain[x+1][y+1].rnory, terrain[x+1][y+1].rnorz));
			uvs.push_back(glm::vec2(terrain[x+1][y+1].s, terrain[x+1][y+1].t));
			vertices.push_back(glm::vec3(terrain[x+1][y+1].rx, terrain[x+1][y+1].ry, terrain[x+1][y+1].rz));
			
			normals.push_back(glm::vec3(terrain[x + 1][y].rnorx, terrain[x + 1][y].rnory, terrain[x + 1][y].rnorz));
			uvs.push_back(glm::vec2(terrain[x + 1][y].s, terrain[x + 1][y].t));
			vertices.push_back(glm::vec3(terrain[x + 1][y].rx, terrain[x + 1][y].ry, terrain[x + 1][y].rz));

			normals.push_back(glm::vec3(terrain[x][y + 1].rnorx, terrain[x][y + 1].rnory, terrain[x][y + 1].rnorz));
			uvs.push_back(glm::vec2(terrain[x][y + 1].s, terrain[x][y + 1].t));
			vertices.push_back(glm::vec3(terrain[x][y + 1].rx, terrain[x][y + 1].ry, terrain[x][y + 1].rz));
		}
	}

	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	*/
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> uvhs;
	std::vector<glm::vec3> normals;

	for (int y = 0;y < TERRAINWIDTH - 1;y++)
	{
		for (int x = 0;x < TERRAINHEIGHT - 1;x++)
		{
			vertices.push_back(glm::vec3(terrain[x][y].rx, terrain[x][y].ry, terrain[x][y].rz));
			normals.push_back(glm::vec3(terrain[x][y].rnorx, terrain[x][y].rnory, terrain[x][y].rnorz));
			uvhs.push_back(glm::vec3(terrain[x][y].s, terrain[x][y].t, terrain[x][y].h));

			vertices.push_back(glm::vec3(terrain[x + 1][y].rx, terrain[x + 1][y].ry, terrain[x + 1][y].rz));
			normals.push_back(glm::vec3(terrain[x + 1][y].rnorx, terrain[x + 1][y].rnory, terrain[x + 1][y].rnorz));
			uvhs.push_back(glm::vec3(terrain[x + 1][y].s, terrain[x + 1][y].t, terrain[x+1][y].h));

			vertices.push_back(glm::vec3(terrain[x][y + 1].rx, terrain[x][y + 1].ry, terrain[x][y + 1].rz));
			normals.push_back(glm::vec3(terrain[x][y + 1].rnorx, terrain[x][y + 1].rnory, terrain[x][y + 1].rnorz));
			uvhs.push_back(glm::vec3(terrain[x][y + 1].s, terrain[x][y + 1].t, terrain[x][y+1].h));

			vertices.push_back(glm::vec3(terrain[x + 1][y + 1].rx, terrain[x + 1][y + 1].ry, terrain[x + 1][y + 1].rz));
			normals.push_back(glm::vec3(terrain[x + 1][y + 1].rnorx, terrain[x + 1][y + 1].rnory, terrain[x + 1][y + 1].rnorz));
			uvhs.push_back(glm::vec3(terrain[x + 1][y + 1].s, terrain[x + 1][y + 1].t, terrain[x+1][y+1].h));

			vertices.push_back(glm::vec3(terrain[x][y + 1].rx, terrain[x][y + 1].ry, terrain[x][y + 1].rz));
			normals.push_back(glm::vec3(terrain[x][y + 1].rnorx, terrain[x][y + 1].rnory, terrain[x][y + 1].rnorz));
			uvhs.push_back(glm::vec3(terrain[x][y + 1].s, terrain[x][y + 1].t, terrain[x][y + 1].h));

			vertices.push_back(glm::vec3(terrain[x + 1][y].rx, terrain[x + 1][y].ry, terrain[x + 1][y].rz));
			normals.push_back(glm::vec3(terrain[x + 1][y].rnorx, terrain[x + 1][y].rnory, terrain[x + 1][y].rnorz));
			uvhs.push_back(glm::vec3(terrain[x + 1][y].s, terrain[x + 1][y].t, terrain[x+1][y].h));
		}
	}

	/*for (int y = 0;y < TERRAINWIDTH - 1;y++)
	{
		vertices.push_back(glm::vec3(terrain[TERRAINHEIGHT-1][y].rx, terrain[TERRAINHEIGHT-1][y].ry, terrain[TERRAINHEIGHT-1][y].rz));
		normals.push_back(glm::vec3(terrain[TERRAINHEIGHT-1][y].rnorx, terrain[TERRAINHEIGHT-1][y].rnory, terrain[TERRAINHEIGHT-1][y].rnorz));
		uvhs.push_back(glm::vec3(terrain[TERRAINHEIGHT-1][y].s, terrain[TERRAINHEIGHT-1][y].t, terrain[TERRAINHEIGHT-1][y].h));

		vertices.push_back(glm::vec3(terrain[0][y].rx, terrain[0][y].ry, terrain[0][y].rz));
		normals.push_back(glm::vec3(terrain[0][y].rnorx, terrain[0][y].rnory, terrain[0][y].rnorz));
		uvhs.push_back(glm::vec3(terrain[0][y].s, terrain[0][y].t, terrain[0][y].h));

		vertices.push_back(glm::vec3(terrain[TERRAINHEIGHT-1][y + 1].rx, terrain[TERRAINHEIGHT-1][y + 1].ry, terrain[TERRAINHEIGHT-1][y + 1].rz));
		normals.push_back(glm::vec3(terrain[TERRAINHEIGHT-1][y + 1].rnorx, terrain[TERRAINHEIGHT-1][y + 1].rnory, terrain[TERRAINHEIGHT-1][y + 1].rnorz));
		uvhs.push_back(glm::vec3(terrain[TERRAINHEIGHT-1][y + 1].s, terrain[TERRAINHEIGHT-1][y + 1].t, terrain[TERRAINHEIGHT-1][y + 1].h));

		vertices.push_back(glm::vec3(terrain[0][y + 1].rx, terrain[0][y + 1].ry, terrain[0][y + 1].rz));
		normals.push_back(glm::vec3(terrain[0][y + 1].rnorx, terrain[0][y + 1].rnory, terrain[0][y + 1].rnorz));
		uvhs.push_back(glm::vec3(terrain[0][y + 1].s, terrain[0][y + 1].t, terrain[0][y + 1].h));

		vertices.push_back(glm::vec3(terrain[01][y].rx, terrain[0][y].ry, terrain[0][y].rz));
		normals.push_back(glm::vec3(terrain[0][y].rnorx, terrain[0][y].rnory, terrain[0][y].rnorz));
		uvhs.push_back(glm::vec3(terrain[0][y].s, terrain[0][y].t, terrain[0][y].h));

		vertices.push_back(glm::vec3(terrain[TERRAINHEIGHT-1][y + 1].rx, terrain[TERRAINHEIGHT-1][y + 1].ry, terrain[TERRAINHEIGHT-1][y + 1].rz));
		normals.push_back(glm::vec3(terrain[TERRAINHEIGHT-1][y + 1].rnorx, terrain[TERRAINHEIGHT-1][y + 1].rnory, terrain[TERRAINHEIGHT-1][y + 1].rnorz));
		uvhs.push_back(glm::vec3(terrain[TERRAINHEIGHT-1][y + 1].s, terrain[TERRAINHEIGHT-1][y + 1].t, terrain[TERRAINHEIGHT-1][y + 1].h));
	}

	for (int x = 0;x < TERRAINHEIGHT - 1;x++)
	{
		vertices.push_back(glm::vec3(terrain[x][TERRAINWIDTH-1].rx, terrain[x][TERRAINWIDTH-1].ry, terrain[x][TERRAINWIDTH-1].rz));
		normals.push_back(glm::vec3(terrain[x][TERRAINWIDTH-1].rnorx, terrain[x][TERRAINWIDTH-1].rnory, terrain[x][TERRAINWIDTH-1].rnorz));
		uvhs.push_back(glm::vec3(terrain[x][TERRAINWIDTH-1].s, terrain[x][TERRAINWIDTH-1].t, terrain[x][TERRAINWIDTH-1].h));

		vertices.push_back(glm::vec3(terrain[x + 1][TERRAINWIDTH-1].rx, terrain[x + 1][TERRAINWIDTH-1].ry, terrain[x + 1][TERRAINWIDTH-1].rz));
		normals.push_back(glm::vec3(terrain[x + 1][TERRAINWIDTH-1].rnorx, terrain[x + 1][TERRAINWIDTH-1].rnory, terrain[x + 1][TERRAINWIDTH-1].rnorz));
		uvhs.push_back(glm::vec3(terrain[x + 1][TERRAINWIDTH-1].s, terrain[x + 1][TERRAINWIDTH-1].t, terrain[x + 1][TERRAINWIDTH-1].h));

		vertices.push_back(glm::vec3(terrain[x][0].rx, terrain[x][0].ry, terrain[x][0].rz));
		normals.push_back(glm::vec3(terrain[x][0].rnorx, terrain[x][0].rnory, terrain[x][0].rnorz));
		uvhs.push_back(glm::vec3(terrain[x][0].s, terrain[x][0].t, terrain[x][0].h));

		vertices.push_back(glm::vec3(terrain[x + 1][0].rx, terrain[x + 1][0].ry, terrain[x + 1][0].rz));
		normals.push_back(glm::vec3(terrain[x + 1][0].rnorx, terrain[x + 1][0].rnory, terrain[x + 1][0].rnorz));
		uvhs.push_back(glm::vec3(terrain[x + 1][0].s, terrain[x + 1][0].t, terrain[x + 1][0].h));

		vertices.push_back(glm::vec3(terrain[x + 1][TERRAINWIDTH-1].rx, terrain[x + 1][TERRAINWIDTH-1].ry, terrain[x + 1][TERRAINWIDTH-1].rz));
		normals.push_back(glm::vec3(terrain[x + 1][TERRAINWIDTH-1].rnorx, terrain[x + 1][TERRAINWIDTH-1].rnory, terrain[x + 1][TERRAINWIDTH-1].rnorz));
		uvhs.push_back(glm::vec3(terrain[x + 1][TERRAINWIDTH-1].s, terrain[x + 1][TERRAINWIDTH-1].t, terrain[x + 1][TERRAINWIDTH-1].h));

		vertices.push_back(glm::vec3(terrain[x][0].rx, terrain[x][0].ry, terrain[x][0].rz));
		normals.push_back(glm::vec3(terrain[x][0].rnorx, terrain[x][0].rnory, terrain[x][0].rnorz));
		uvhs.push_back(glm::vec3(terrain[x][0].s, terrain[x][0].t, terrain[x][0].h));
	}
	*/
	indexVBO(vertices, uvhs, normals, indices, indexed_vertices, indexed_uvhs, indexed_normals);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvhbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvhbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvhs.size() * sizeof(glm::vec3), &indexed_uvhs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	/*
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> uvhs;
	std::vector<glm::vec3> normals;

	for (int y = 0;y < TERRAINWIDTH - 1;y++)
	{
		for (int x = 0;x < TERRAINHEIGHT - 1;x++)
		{
			vertices.push_back(glm::vec3(terrain[x][y].x, terrain[x][y].y, terrain[x][y].z));
			normals.push_back(glm::vec3(terrain[x][y].norx, terrain[x][y].nory, terrain[x][y].norz));
			uvhs.push_back(glm::vec3(terrain[x][y].s, terrain[x][y].t, terrain[x][y].h));

			vertices.push_back(glm::vec3(terrain[x + 1][y].x, terrain[x + 1][y].y, terrain[x + 1][y].z));
			normals.push_back(glm::vec3(terrain[x + 1][y].norx, terrain[x + 1][y].nory, terrain[x + 1][y].norz));
			uvhs.push_back(glm::vec3(terrain[x + 1][y].s, terrain[x + 1][y].t, terrain[x + 1][y].h));

			vertices.push_back(glm::vec3(terrain[x][y + 1].x, terrain[x][y + 1].y, terrain[x][y + 1].z));
			normals.push_back(glm::vec3(terrain[x][y + 1].norx, terrain[x][y + 1].nory, terrain[x][y + 1].norz));
			uvhs.push_back(glm::vec3(terrain[x][y + 1].s, terrain[x][y + 1].t, terrain[x][y + 1].h));

			vertices.push_back(glm::vec3(terrain[x + 1][y + 1].x, terrain[x + 1][y + 1].y, terrain[x + 1][y + 1].z));
			normals.push_back(glm::vec3(terrain[x + 1][y + 1].norx, terrain[x + 1][y + 1].nory, terrain[x + 1][y + 1].norz));
			uvhs.push_back(glm::vec3(terrain[x + 1][y + 1].s, terrain[x + 1][y + 1].t, terrain[x + 1][y + 1].h));

			vertices.push_back(glm::vec3(terrain[x + 1][y].x, terrain[x + 1][y].y, terrain[x + 1][y].z));
			normals.push_back(glm::vec3(terrain[x + 1][y].norx, terrain[x + 1][y].nory, terrain[x + 1][y].norz));
			uvhs.push_back(glm::vec3(terrain[x + 1][y].s, terrain[x + 1][y].t, terrain[x + 1][y].h));

			vertices.push_back(glm::vec3(terrain[x][y + 1].x, terrain[x][y + 1].y, terrain[x][y + 1].z));
			normals.push_back(glm::vec3(terrain[x][y + 1].norx, terrain[x][y + 1].nory, terrain[x][y + 1].norz));
			uvhs.push_back(glm::vec3(terrain[x][y + 1].s, terrain[x][y + 1].t, terrain[x][y + 1].h));
		}
	}

	indexVBO(vertices, uvhs, normals, indices, indexed_vertices, indexed_uvhs, indexed_normals);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvhbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvhbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvhs.size() * sizeof(glm::vec3), &indexed_uvhs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	*/
}