#pragma once
#ifndef __draw_h__
#define __draw_h__

#include "ball.h"
#include "flag.h"
#include "particle.h"
#include "skybox.h"

void glutWireSphere(ball &b);
void glutSolidSphere(ball &b);
void glutWireRec(float length, float height, float width);
void glutSolidRec(float length, float height, float width);
void glutSolidFlag(flag &f);
int DrawGLScene(particleEngine & pe);	// »æÖÆÁ£×Ó
void DrawSkyBox(skybox & s);
void DrawTerrain();

#endif