#pragma once
#ifndef __impact_h__
#define __impact_h__

#include "ball.h"
#include <map>
using namespace std;

bool impact_detect(ball &b1, ball &b2);

void reverse_speed_y(ball &b);
void reverse_speed_x(ball & b);

int impact_detect(ball &b);
void detect_reverse(ball & b);

pair<float, float>& vector_projection(const pair<float, float>& v1, const pair<float, float>& v2);
void change_speed(ball &b1, ball &b2);

#endif