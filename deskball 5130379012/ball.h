#ifndef __ball_h__
#define __ball_h__
struct centerPoint
{
	float x;
	float y;
	float z;
};

struct speed
{
	float x;
	float y;
	float z;
};

struct acceleration
{
	float x;
	float y;
};

struct ball
{
	float radius;
	centerPoint p;
	speed v;
	acceleration a;
	bool sp;
};
#endif