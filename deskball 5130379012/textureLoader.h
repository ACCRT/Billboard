#pragma once

#ifndef _TEXTURE_LOADER_
#define _TEXTURE_LOADER_
#include<GL\glew.h>
#include"FreeImage.h"
bool LoadTexture(const char* filename,	//where to load the file from
	GLuint &texID,
	//does not have to be generated with glGenTextures
	GLenum image_format = GL_BGR,		//format the image is in
	GLint internal_format = GL_RGB,		//format to store the image in
	GLint level = 0 ,					//mipmapping level
	GLint border = 0);

bool GenTexture2D(BYTE* bits, GLuint &texID, int width, int height);
bool GenTexture3D(BYTE* bits, GLuint &texID, int width, int height, int depth);
#endif