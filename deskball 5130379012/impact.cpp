#include "impact.h"

bool impact_detect(ball &b1, ball &b2)
{
	return (sqrt(pow(b1.p.x - b2.p.x, 2) + pow((b1.p.y - b2.p.y), 2) + pow((b1.p.z - b2.p.z), 2)) <= b1.radius + b2.radius);
}


void reverse_speed_y(ball &b)
{
	b.v.y = -b.v.y;
}
/*
void detect_reverse(ball & b)
{
	int detect;
	if (b.p.x <= 0) b.p.x = 1079;
	if (b.p.x >= 1080) b.p.x = 0;
	if (b.p.y >= 1080) b.p.y = 0;
	if (b.p.y <= 0) b.p.y = 1079;
}*/
int impact_detect(ball &b)
{
	bool detect_x = false;
	bool detect_y = false;
	if (abs(b.p.x - 1080) < b.radius || abs(b.p.x - 0) < b.radius || b.p.x>1080 + b.radius || b.p.x<-b.radius)
		detect_x = true;
	if (abs(b.p.y - 1080) < b.radius || abs(b.p.y - 0) < b.radius || b.p.y>1080 + b.radius || b.p.y<-b.radius)
		detect_y = true;

	if (detect_x && detect_y) return 3;
	if (detect_x) return 1;
	if (detect_y) return 2;
	return 0;
}

void reverse_speed_x(ball & b)
{
	b.v.x = -b.v.x;
}

void detect_reverse(ball & b)
{
	/*int detect;
	detect = impact_detect(b);
	if (detect == 3)
	{
		reverse_speed_x(b);
		reverse_speed_y(b);
	}
	else if (detect == 2) reverse_speed_y(b);
	else if (detect == 1) reverse_speed_x(b);*/
	if (b.p.y < 0)
	{
		b.p.y = -b.p.y;
		b.p.x = b.p.x + 540;
		b.v.y = -b.v.y;
	}
	if (b.p.y - 1080 >= 0)
	{
		b.p.y =1080-(b.p.y - 1080);
		b.p.x = b.p.x + 540;
		b.v.y = -b.v.y;
	}
	if (b.p.x - 1080 >= 0)
		b.p.x = b.p.x - 1080;
	if (b.p.x < 0)
		b.p.x = 1080 + b.p.x;

}


pair<float, float>& vector_projection(const pair<float, float>& v1, const pair<float, float>& v2)
{
	double v1_length = sqrt(pow(v1.first, 2) + pow(v1.second, 2));
	double v2_length = sqrt(pow(v2.first, 2) + pow(v2.second, 2));

	pair<float, float> v2_unit(v2.first / v2_length, v2.second / v2_length);
	double result_length = v1.first* v2_unit.first + v1.second*v2_unit.second;
	pair<float, float> result;

	result.first = result_length*v2_unit.first;
	result.second = result_length*v2_unit.second;

	return result;
}

void change_speed(ball &b1, ball &b2)
{
	pair<float, float> x_12(b1.p.x - b2.p.x, b1.p.y - b2.p.y);

	pair<float, float> v_1(b1.v.x, b1.v.y);
	pair<float, float> v_2(b2.v.x, b2.v.y);

	double old_en = 1 / 2 * pow(v_1.first, 2) + 1 / 2 * pow(v_1.second, 2) + 1 / 2 * pow(v_2.first, 2) + 1 / 2 * pow(v_2.second, 2);

	pair<float, float> new_v1_line = vector_projection(v_1, x_12);
	pair<float, float> new_v2_line = vector_projection(v_2, x_12);

	b1.v.x = new_v2_line.first + v_1.first - new_v1_line.first;
	b1.v.y = new_v2_line.second + v_1.second - new_v1_line.second;
	b2.v.x = new_v1_line.first + v_2.first - new_v2_line.first;
	b2.v.y = new_v1_line.second + v_2.second - new_v2_line.second;

	double old_en2 = 1 / 2 * pow(b1.v.x, 2) + 1 / 2 * pow(b1.v.y, 2) + 1 / 2 * pow(b2.v.x, 2) + 1 / 2 * pow(b2.v.y, 2);
	//cout << old_en - old_en2<<endl;
}