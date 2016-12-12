#pragma once

#define TERRAINHEIGHT 1080
#define TERRAINWIDTH 1080

struct Terrain
{
	double x, y, z;
	float s, t, h;//纹理坐标
	float norx, nory, norz;
	double rx, ry, rz;
	float rnorx, rnory, rnorz;
};

//地形使用TERRAINHEIGHT*TERRAINWIDTH的点阵
extern Terrain terrain[TERRAINHEIGHT][TERRAINWIDTH];
extern GLint terrain_colors[14*6][3];

void initTerrain();