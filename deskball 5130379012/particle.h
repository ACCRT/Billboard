#pragma once
#include <GL/glut.h>
//#include "color.h"
#define	MAX_PARTICLES	1000		// 定义最大的粒子数
//extern GLint blue700Color[3];
struct particles
{
	bool	active;				// 是否激活
	float	life;				// 粒子生命
	float	fade;				// 衰减速度

	GLint	r;					// 红色值
	GLint	g;					// 绿色值
	GLint	b;					// 蓝色值

	float	x;					// X 位置
	float	y;					// Y 位置
	float	z;					// Z 位置

	float	xi;					// X 方向
	float	yi;					// Y 方向
	float	zi;					// Z 方向

	float	xg;					// X 方向重力加速度
	float	yg;					// Y 方向重力加速度
	float	zg;					// Z 方向重力加速度
};



struct particleEngine
{
	int particlenumber;
	bool	rainbow = true;		// 是否为彩虹模式

	float	slowdown = 2.0f;	// 减速粒子
	float	xspeed;				// X方向的速度
	float	yspeed;				// Y方向的速度
	float	zoom = -40.0f;		// 沿Z轴缩放

	GLuint	loop;				// 循环变量
	GLuint	col;				// 当前的颜色
	GLuint	delay;				// 彩虹效果延迟

	GLuint vertex_id;			// 粒子系统纹理

	particles particle[MAX_PARTICLES];	// 保存1000个粒子的数组

	/*GLint colors[12][3] =				// 彩虹颜色
	{
		{ 231,233,253 },{ 208,217,255 },{ 166 ,186,255 },{ 175,191 ,255 },
		{ 104,137 ,255  },{ 145 ,167 ,255  },{ 115 ,143 ,254 },{ 77 ,115 ,255 },
		{ 86 ,119 ,252 },{ 78,108,239 },{ 69,94,222 },{ 42 ,54 ,177 }
	};*/
	GLint colors[12][3];				// 彩虹颜色
	/*{
		{ 0x45, 0x5e, 0xde },{ 0x45, 0x5e, 0xde },{ 0x45, 0x5e, 0xde },{ 0x45, 0x5e, 0xde },
		{ 0x45, 0x5e, 0xde },{ 0x45, 0x5e, 0xde },blue700Color[0],blue700Color[0],
		blue700Color[0],blue700Color[0],blue700Color[0],blue700Color[0]
	};*/
	
};