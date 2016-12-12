#pragma once
#include <GL/glut.h>
//#include "color.h"
#define	MAX_PARTICLES	1000		// ��������������
//extern GLint blue700Color[3];
struct particles
{
	bool	active;				// �Ƿ񼤻�
	float	life;				// ��������
	float	fade;				// ˥���ٶ�

	GLint	r;					// ��ɫֵ
	GLint	g;					// ��ɫֵ
	GLint	b;					// ��ɫֵ

	float	x;					// X λ��
	float	y;					// Y λ��
	float	z;					// Z λ��

	float	xi;					// X ����
	float	yi;					// Y ����
	float	zi;					// Z ����

	float	xg;					// X �����������ٶ�
	float	yg;					// Y �����������ٶ�
	float	zg;					// Z �����������ٶ�
};



struct particleEngine
{
	int particlenumber;
	bool	rainbow = true;		// �Ƿ�Ϊ�ʺ�ģʽ

	float	slowdown = 2.0f;	// ��������
	float	xspeed;				// X������ٶ�
	float	yspeed;				// Y������ٶ�
	float	zoom = -40.0f;		// ��Z������

	GLuint	loop;				// ѭ������
	GLuint	col;				// ��ǰ����ɫ
	GLuint	delay;				// �ʺ�Ч���ӳ�

	GLuint vertex_id;			// ����ϵͳ����

	particles particle[MAX_PARTICLES];	// ����1000�����ӵ�����

	/*GLint colors[12][3] =				// �ʺ���ɫ
	{
		{ 231,233,253 },{ 208,217,255 },{ 166 ,186,255 },{ 175,191 ,255 },
		{ 104,137 ,255  },{ 145 ,167 ,255  },{ 115 ,143 ,254 },{ 77 ,115 ,255 },
		{ 86 ,119 ,252 },{ 78,108,239 },{ 69,94,222 },{ 42 ,54 ,177 }
	};*/
	GLint colors[12][3];				// �ʺ���ɫ
	/*{
		{ 0x45, 0x5e, 0xde },{ 0x45, 0x5e, 0xde },{ 0x45, 0x5e, 0xde },{ 0x45, 0x5e, 0xde },
		{ 0x45, 0x5e, 0xde },{ 0x45, 0x5e, 0xde },blue700Color[0],blue700Color[0],
		blue700Color[0],blue700Color[0],blue700Color[0],blue700Color[0]
	};*/
	
};