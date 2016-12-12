// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.h"

#include <GL/glut.h>
#include "light.h"
#include "ball.h"
#include <map>
#include <iostream>
#include <vector>

using namespace std;
extern Light light[2];

#define B1_NUM			6
#define B2_NUM			6
#define PI 3.1415926535

extern ball b_1[B1_NUM];
extern ball b_2[B2_NUM];
extern ball sp;
extern ball cue;

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

int following = -1;
int verynear = 0;
int change_version = 0;

#define lerp(t, a, b) ( a + t * (b - a) )	// ÏßÐÔº¯Êý

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 0 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 30.0f; // 3 units / second
float mouseSpeed = 0.0005f;

void handleKeys()
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if(light[1].open)
			light[1].open = false;
		else light[1].open = true;
	}
}

void follow_input()
{
	// 48 GLFW_KEY_0
	// 58 GLFW_KEY_9
	for (int i = 48;i < 58;i++)
	{
		if (glfwGetKey(window, i) == GLFW_PRESS)
			following = i-48;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		following = 10;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		following = 11;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		following = 12;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		following = 13;

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		following = -1;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		verynear = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		verynear = 0;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		change_version = 1;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		change_version = 0;
}

double abs_(double x)
{
	if (x < 0) return -x;
	else return x;
}

double max_(double x, double y)
{
	if (x > y) return x;
	else return y;
}

void computeMatricesFromInputs() {

	follow_input();

	if (following == -1)
	{
		// glfwGetTime is called only once, the first time this function is called
		static double lastTime = glfwGetTime();

		// Compute time difference between current and last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);

		// Get mouse position
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// Reset mouse position for next frame
		glfwSetCursorPos(window, 1024 / 2, 768 / 2);

		// Compute new orientation
		horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
		verticalAngle += mouseSpeed * float(768 / 2 - ypos);

		// Direction : Spherical coordinates to Cartesian coordinates conversion
		glm::vec3 direction(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
			);

		// Right vector
		glm::vec3 right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f)
			);

		// Up vector
		glm::vec3 up = glm::cross(right, direction);


		// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
		// Camera matrix
		ViewMatrix = glm::lookAt(
			position,           // Camera is here
			position + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
			);

		glLoadIdentity();
		gluLookAt(-600.0, -600.0, 600.0, 0.0, 0.0, 0.0, 1, 1, 1);
		gluLookAt(position.x, position.y, position.z, (position + direction).x, (position + direction).y, (position + direction).z, up.x, up.y, up.z);
		//cout << position.x << " " << position.y << position.z<<endl;

		// For the next frame, the "last time" will be "now"
		lastTime = currentTime;
		return;
	}
	
	float posx, posy, posz;
	float posrx, posry, posrz;
	float vx, vy, vz;
	float vrx, vry, vrz;
	float direcx=0, direcy=0, direcz=0;
	if (following >= 0)
	{

		posx = b_1[following].p.x;
		posy = b_1[following].p.y;
		posz = 800;
		if (verynear)
		{
			posx = b_1[following].p.x - 120;
			posy = b_1[following].p.y - (b_1[following].v.y) / abs(b_1[following].v.y) * 120;
			if (change_version)
			{
				posx = b_1[following].p.x - min(200.0, (200.0 / abs_(b_1[following].v.x)));
				posy = b_1[following].p.y - (b_1[following].v.y) / abs(b_1[following].v.y) *min(200.0, (200.0 / abs_(b_1[following].v.y)));
			}
			posz = 510;
			direcx = (b_1[following].p.z + 100)* cos(b_1[following].p.x / 1080.0 * 2 * PI)*sin(b_1[following].p.y / 1080.0  * PI);
			direcy = (b_1[following].p.z + 100)* sin(b_1[following].p.x / 1080.0 * 2 * PI)*sin(b_1[following].p.y / 1080.0 * PI);
			direcz = (b_1[following].p.z + 100)* cos(b_1[following].p.y / 1080.0 * PI);
		}
		vx = b_1[following].v.x;
		vy = b_1[following].v.y;
		vz = (b_1[following].v.y) / abs(b_1[following].v.y) * 500;
	}
	if (following >= 6 && following<12)
	{
		posx = b_2[following-6].p.x;
		posy = b_2[following-6].p.y;
		posz = 800;
		if (verynear)
		{
			posx = b_2[following-6].p.x - 120;
			posy = b_2[following-6].p.y - (b_2[following-6].v.y) / abs(b_2[following - 6].v.y) * 120;
			if (change_version)
			{
				posx = b_2[following-6].p.x - min(200.0, (200.0 / abs_(b_2[following-6].v.x)));
				posy = b_2[following-6].p.y - (b_2[following - 6].v.y) / abs(b_2[following - 6].v.y)*min(200.0, (200.0 / abs_(b_2[following-6].v.y)));
			}
			posz = 510;
			direcx = (b_2[following-6].p.z+100)* cos(b_2[following-6].p.x / 1080.0 * 2 * PI)*sin(b_2[following-6].p.y / 1080.0 * PI);
			direcy = (b_2[following - 6].p.z + 100)*sin(b_2[following-6].p.x / 1080.0 * 2 * PI)*sin(b_2[following-6].p.y / 1080.0  * PI);
			direcz = (b_2[following - 6].p.z + 100)* cos(b_2[following-6].p.y / 1080.0 * PI);;
		}
		vx = b_2[following-6].v.x;
		vy = b_2[following-6].v.y;
		vz = (b_2[following - 6].v.y) / abs(b_2[following - 6].v.y) * 500;
	}

	if (following == 12)
	{
		posx = sp.p.x;
		posy = sp.p.y;
		posz = 800;
		if (verynear)
		{
			posx = sp.p.x - 120;
			posy = sp.p.y - (sp.v.y) / abs(sp.v.y) * 120;
			posz = sp.p.z + 100;
			if (change_version)
			{
				posx = sp.p.x - min(200.0, (200.0 / abs_(sp.v.x)));
				posy = sp.p.y - (sp.v.y) / abs(sp.v.y) *min(200.0, (200.0 / abs_(sp.v.y)));
			}
			direcx = (sp.p.z + 100)* cos(sp.p.x / 1080.0 * 2 * PI)*sin(sp.p.y / 1080.0  * PI);
			direcy = (sp.p.z + 100)* sin(sp.p.x / 1080.0 * 2 * PI)*sin(sp.p.y / 1080.0 * PI);
			direcz = (sp.p.z + 100)* cos(sp.p.y / 1080.0  * PI);;
		}
		vx = sp.v.x;
		vy = sp.v.y;
		vz = (sp.v.y) / abs(sp.v.y) * 500;
	}

	if (following == 13)
	{
		posx = cue.p.x;
		posy = cue.p.y;
		posz = 800;
		if (verynear)
		{
			posx = cue.p.x - 120;
			posy = cue.p.y - (cue.v.y) / abs(cue.v.y) * 120;
			posz = cue.p.z + 100;
			if (change_version)
			{
				posx = cue.p.x - min(200.0, (200.0 / abs_(cue.v.x)));
				posy = cue.p.y - (cue.v.y) / abs(cue.v.y) *min(200.0, (200.0 / abs_(cue.v.y)));
			}
			direcx = (cue.p.z + 100)* cos(cue.p.x / 1080.0 * 2 * PI)*sin(cue.p.y / 1080.0  * PI);
			direcy = (cue.p.z + 100)* sin(cue.p.x / 1080.0 * 2 * PI)*sin(cue.p.y / 1080.0 * PI);
			direcz = (cue.p.z + 100)* cos(cue.p.y / 1080.0  * PI);;
		}
		vx = cue.v.x;
		vy = cue.v.y;
		vz = (cue.v.y) / abs(cue.v.y) * 500;
	}

	posrx = posz * cos(posx / 1080.0 * 2 * PI)*sin(posy / 1080.0  * PI);
	posry = posz * sin(posx / 1080.0 * 2 * PI)*sin(posy / 1080.0  * PI);
	posrz = posz * cos(posy / 1080.0  * PI);

	//cout << cos(2 * PI) << " " << cos(0) << endl;
	vrx = vz * cos(vx / 1080.0 * 2 * PI)*sin(vy / 1080.0 * PI);
	vry = vz * sin(vx / 1080.0 * 2 * PI)*sin(vy / 1080.0 * PI);
	vrz = vz * cos(vy / 1080.0  * PI);
	//if (vy >= 0 && vy <= 540)
	//{
	//	vrx = -vrx;
	//	vry = -vry;
	//	vrz = -vrz;
	//}
	
	//cout << vrx << " " << vry << " " << vrz << endl;
	//cout << posx << " " << posy <<" "<< posz<< endl;

	if (verynear)
	{
		//vrx = cos(posx / 1080.0 * 2 * PI)*sin(posy / 1080.0 * 2 * PI);
		//vry = sin(posx / 1080.0 * 2 * PI)*sin(posy / 1080.0 * 2 * PI);
		//vrz = cos(posy / 1080.0 * 2 * PI);
		vrx = lerp(0.5, posrx, direcx);
		vry = lerp(0.5, posry, direcy);
		vrz = lerp(0.5, posrz, direcz);
	}


	glLoadIdentity();
	gluLookAt(posrx, posry, posrz, direcx, direcy, direcz, vrx, vry, vrz);
}