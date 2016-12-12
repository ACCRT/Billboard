#include <iostream>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;
#include "perlin.h";
#include "terrain.h"
#include "color.h"
#include <gl\glut.h>

#define PI 3.1415926

//每个点的高度使用perlin函数生成
Terrain terrain[TERRAINHEIGHT][TERRAINWIDTH];
GLint terrain_colors[14*6][3];

void initTerrain()
{
	int x, y;
	for (x = 0;x < TERRAINHEIGHT;x++)
		for (y = 0;y < TERRAINWIDTH;y++)
		{
			terrain[x][y].x = (float)x * 1080 / TERRAINHEIGHT;
			terrain[x][y].y = (float)y * 1080 / TERRAINWIDTH;

			double vec[2] = { 0.001*abs(x-540), 0.001*abs(y-540) };
			terrain[x][y].z = 100.0f*(PerlinNoise2D(0.01*abs(x-540),0.01* abs(y - 540),2,2,4)+1)-100.0f;
			//terrain[x][y].z = 100.0f*(noise2(vec) + 1) - 100.0f;

			terrain[x][y].s = float(x) / TERRAINHEIGHT;
			terrain[x][y].t = float(y) / TERRAINWIDTH;
			terrain[x][y].h = (float(terrain[x][y].z)+100.0) / 200.0;

			//terrain[x][y].z = 400;
			//terrain[x][y].h = 0.5;
			//cout << terrain[x][y].z << endl;
			terrain[x][y].z += 400;
			terrain[x][y].rx = (terrain[x][y].z) * cos(x / 1080.0 * 2.0 * PI)*sin(y / 1080.0 * PI);
			terrain[x][y].ry = (terrain[x][y].z) * sin(x / 1080.0 * 2.0 * PI)*sin(y / 1080.0  * PI);
			terrain[x][y].rz = (terrain[x][y].z) * cos(y / 1080.0 * PI);
			
			//if(terrain[x][y].rz != (terrain[x][y].z) * cos(y / 1080.0 * PI))
			//	cout << (terrain[x][y].z) * cos(y / 1080.0 * PI) - terrain[x][y].rz << endl;

			//rain[x][y].s = (sin(float(x) / TERRAINHEIGHT * 2 * PI)+1)/2;
			//terrain[x][y].t = (sin(float(y) / TERRAINHEIGHT * 2 * PI)+2)/2;
			//terrain[x][y].h = (sin((float(terrain[x][y].z) + 100.0) / 200.0)+1)/2;
		}
	
	//for (x = 0; x < TERRAINHEIGHT; x++) {
	//	for (y = 0; y < TERRAINWIDTH; y++) {
	//		//terrain[x][y].z = (terrain[x][y].z - min) / (max - min) * 200 - 100;
	//	}
	//}

	for (x = 0;x < TERRAINHEIGHT;x++)
		for (y = 0;y < TERRAINWIDTH;y++)
		{
			terrain[x][y].rnorx = terrain[x][y].rx;
			terrain[x][y].rnory = terrain[x][y].ry;
			terrain[x][y].rnorz = terrain[x][y].rz;

			double norlength = sqrt(terrain[x][y].rnorx*terrain[x][y].rnorx + terrain[x][y].rnory*terrain[x][y].rnory + terrain[x][y].rnorz*terrain[x][y].rnorz);
			terrain[x][y].rnorx = terrain[x][y].rnorx / norlength;
			terrain[x][y].rnory = terrain[x][y].rnory / norlength;
			terrain[x][y].rnorz = terrain[x][y].rnorz / norlength;

			if (x>0 && y>0 && x<TERRAINHEIGHT && y<TERRAINWIDTH)//法向计算
			{
				glm::vec3 v1(
					terrain[x + 1][y].z - terrain[x - 1][y].z,
					terrain[x][y + 1].z - terrain[x][y - 1].z,
					0.5f
				);
				double length = sqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z);
				v1.x = v1.x / length;
				v1.y = v1.y / length;
				v1.z = v1.z / length;
				//cout << sqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z) << endl;
				if (v1.z > 0)
				{
					terrain[x][y].norx = -v1.x;
					terrain[x][y].nory = -v1.y;
					terrain[x][y].norz = -v1.z;
				}
				else
				{
					terrain[x][y].norx = v1.x;
					terrain[x][y].nory = v1.y;
					terrain[x][y].norz = v1.z;
				}
				/*if (terrain[x][y].rnorz < 0 && terrain[x][y].z >0 ||(terrain[x][y].rnorz >0 && terrain[x][y].z <0))
				{
					terrain[x][y].rnorx = -terrain[x][y].rnorx;
					terrain[x][y].rnory = -terrain[x][y].rnory;
					terrain[x][y].rnorz = -terrain[x][y].rnorz;
				}*/
				
			}
			else
			{
				terrain[x][y].norx = 0.0f;
				terrain[x][y].nory = 0.0f;
				terrain[x][y].norz = 1.0f;
			}
		}

	for (int i = 0;i < 3;i++)
	{
		terrain_colors[0][i] = indigo50Color[i];
		terrain_colors[1][i] = indigo100Color[i];
		terrain_colors[2][i] = indigoA100Color[i];
		terrain_colors[3][i] = indigo200Color[i];
		terrain_colors[4][i] = indigoA200Color[i];
		terrain_colors[5][i] = indigo300Color[i];
		terrain_colors[6][i] = indigo400Color[i];
		terrain_colors[7][i] = indigoA400Color[i];
		terrain_colors[8][i] = indigo500Color[i];
		terrain_colors[9][i] = indigo600Color[i];
		terrain_colors[10][i] = indigo700Color[i];
		terrain_colors[11][i] = indigoA700Color[i];
		terrain_colors[12][i] = indigo800Color[i];
		terrain_colors[13][i] = indigo900Color[i];

		terrain_colors[14][i] = blue900Color[i];
		terrain_colors[15][i] = blue800Color[i];
		terrain_colors[16][i] = blueA700Color[i];
		terrain_colors[17][i] = blue700Color[i];
		terrain_colors[18][i] = blue600Color[i];
		terrain_colors[19][i] = blue500Color[i];
		terrain_colors[20][i] = blue400Color[i];
		terrain_colors[21][i] = blueA400Color[i];
		terrain_colors[22][i] = blue300Color[i];
		terrain_colors[23][i] = blueA200Color[i];
		terrain_colors[24][i] = blue200Color[i];
		terrain_colors[25][i] = blueA100Color[i];
		terrain_colors[26][i] = blue100Color[i];
		terrain_colors[27][i] = blue50Color[i];

		terrain_colors[28][i] = lightBlue50Color[i];
		terrain_colors[29][i] = lightBlue100Color[i];
		terrain_colors[30][i] = lightBlueA100Color[i];
		terrain_colors[31][i] = lightBlue200Color[i];
		terrain_colors[32][i] = lightBlueA200Color[i];
		terrain_colors[33][i] = lightBlue300Color[i];
		terrain_colors[34][i] = lightBlue400Color[i];
		terrain_colors[35][i] = lightBlueA400Color[i];
		terrain_colors[36][i] = lightBlue500Color[i];
		terrain_colors[37][i] = lightBlue600Color[i];
		terrain_colors[38][i] = lightBlue700Color[i];
		terrain_colors[39][i] = lightBlueA700Color[i];
		terrain_colors[40][i] = lightBlue800Color[i];
		terrain_colors[41][i] = lightBlue900Color[i];

		terrain_colors[42][i] = cyan900Color[i];
		terrain_colors[43][i] = cyan800Color[i];
		terrain_colors[44][i] = cyanA700Color[i];
		terrain_colors[45][i] = cyan700Color[i];
		terrain_colors[46][i] = cyan600Color[i];
		terrain_colors[47][i] = cyan500Color[i];
		terrain_colors[48][i] = cyan400Color[i];
		terrain_colors[49][i] = cyanA400Color[i];
		terrain_colors[50][i] = cyan300Color[i];
		terrain_colors[51][i] = cyanA200Color[i];
		terrain_colors[52][i] = cyan200Color[i];
		terrain_colors[53][i] = cyanA100Color[i];
		terrain_colors[54][i] = cyan100Color[i];
		terrain_colors[55][i] = cyan50Color[i];

		terrain_colors[56][i] = teal50Color[i];
		terrain_colors[57][i] = teal100Color[i];
		terrain_colors[58][i] = tealA100Color[i];
		terrain_colors[59][i] = teal200Color[i];
		terrain_colors[60][i] = tealA200Color[i];
		terrain_colors[61][i] = teal300Color[i];
		terrain_colors[62][i] = teal400Color[i];
		terrain_colors[63][i] = tealA400Color[i];
		terrain_colors[64][i] = teal500Color[i];
		terrain_colors[65][i] = teal600Color[i];
		terrain_colors[66][i] = teal700Color[i];
		terrain_colors[67][i] = tealA700Color[i];
		terrain_colors[68][i] = teal800Color[i];
		terrain_colors[69][i] = teal900Color[i];

		terrain_colors[70][i] = green900Color[i];
		terrain_colors[71][i] = green800Color[i];
		terrain_colors[72][i] = greenA700Color[i];
		terrain_colors[73][i] = green700Color[i];
		terrain_colors[74][i] = green600Color[i];
		terrain_colors[75][i] = green500Color[i];
		terrain_colors[76][i] = green400Color[i];
		terrain_colors[77][i] = greenA400Color[i];
		terrain_colors[78][i] = green300Color[i];
		terrain_colors[79][i] = greenA200Color[i];
		terrain_colors[80][i] = green200Color[i];
		terrain_colors[81][i] = greenA100Color[i];
		terrain_colors[82][i] = green100Color[i];
		terrain_colors[83][i] = green50Color[i];
	}

}