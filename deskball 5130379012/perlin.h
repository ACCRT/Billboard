#pragma once
#define B 0x100
#define BM 0xff
#define N 0x1000

#define s_curve(t) ( t * t * t * (t * (t * 6 - 15) + 10) )	// s����
#define lerp(t, a, b) ( a + t * (b - a) )	// ���Ժ���
// b0ΪС��vec[i]������,b1Ϊ����vec[i]������
// r0Ϊ�м�㵽b0�ķ��ž���(Ϊ��),r1Ϊ�м�㵽b1�ķ��ž���(Ϊ��)
#define setup(i,b0,b1,r0,r1)\
        t = vec[i] + N;\
        b0 = ((int)t) & BM;\
        b1 = (b0+1) & BM;\
        r0 = t - (int)t;\
        r1 = r0 - 1.0;
#define at2(rx,ry) ( rx * q[0] + ry * q[1] )	// Ȩֵ���
#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

void pinit(void);
double noise1(double);
double noise2(double *);
double noise3(double *);
void normalize3(double *);
void normalize2(double *);

double PerlinNoise1D(double, double, double, int);
double PerlinNoise2D(double, double, double, double, int);
double PerlinNoise3D(double, double, double, double, double, int);