#pragma once
#ifdef USE_GLES
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif

#include <Game/YugiohUnit.h>

class DebugCard : public Game::YugiohUnit
{
public:

	GLuint statsVBO;
	GLuint statsIBO;
	GLuint statsTBO;
#ifndef NOVA
	GLuint statsVAO;
#endif
	void setup();

	void draw();
};

