#ifdef USE_GLES
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif
#include <Utility/ErrorHandler.h>
#include <Base/RenderUnit.h>
#include <Game/Animation/Camera.h>
#include <Utility/DebugUnit.h>
#include <Utility/TextPrinter.h>
#include <Game/Cursor.h>
#include <Utility/Clock.h>
#include <Game/Animation/FadeUnit.h>
#include <iostream>
#include <Game/Cards/CardDisplayUnit.h>
#include <Game/Animation/ParticlesUnit.h>

#define YUG_DEFAULT_SCREEN_WIDTH 800
#ifdef PANDORA
#define YUG_DEFAULT_SCREEN_HEIGHT 480
#define YUG_WINDOW_ORIGIN_POINT 400,0
#else
#define YUG_DEFAULT_SCREEN_HEIGHT 600
#define YUG_WINDOW_ORIGIN_POINT 500,0
#endif

int frameCount;
float timeCount;
bool RenderUnit::initialize(){
	frameCount = 0;
	timeCount = 0.0f;
	resize(YUG_DEFAULT_SCREEN_WIDTH,YUG_DEFAULT_SCREEN_HEIGHT);
	move(YUG_WINDOW_ORIGIN_POINT);
	oldRenderer = NULL;
	currentRenderer = NULL;
	show();
	return true;
}

void RenderUnit::initializeGL(){
#ifndef USE_GLES
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK)
	{
		errorHandler.printError( "glew Error: ");
		std::cout<<glewGetErrorString(glewError)<<std::endl;
	}else{
		errorHandler.printError("glew on");
	}
#endif
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool RenderUnit::shutdown(){
	return true;
}

void RenderUnit::render(){
	glDraw();
}

void RenderUnit::paintEvent(){
	//blocks QT's auto/uncontrollable repainting
}

void RenderUnit::paintGL(){
	
	frameCount ++;
	timeCount += gameClock.lastLoopTime();
	
	glViewport(0,	0,	width(),	height() );
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	if(currentRenderer != NULL)
		currentRenderer->render();
	else
		errorHandler.printError("RenderUnit: paint: render call called on NULL currentRenderer.");
	puzzleCursor.render();
	//cardDisplayUnit.render();
	fadeUnit.render(YUG_FADE_GLOBAL);
	cardDisplayUnit.render();
	//particleUnit.render();
}

void RenderUnit::newRenderer(Game::YugiohUnit* newRenderer){
	if(newRenderer == NULL){
		errorHandler.printError("Render Unit: passed new renderer, ignoring it.");
	}else{
		oldRenderer = currentRenderer;
		currentRenderer = newRenderer;
	}

}
void RenderUnit::swapBackToOldRenderer(){
	if(oldRenderer == NULL){
		errorHandler.printError("Render Unit: swapping in null pointer, ignoring it.");
	}else{
		Game::YugiohUnit* temp;
		temp = oldRenderer;
		oldRenderer = currentRenderer;
		currentRenderer = temp;
	}
}
void RenderUnit::returnToOldRenderer(){
	if(oldRenderer == NULL){
		errorHandler.printError("Render Unit: returning to null pointer, ignoring it.");
	}else{
		currentRenderer = oldRenderer;
		oldRenderer = NULL;
	}

}
void RenderUnit::setOldRenderer(Game::YugiohUnit* newRenderer){
	oldRenderer = newRenderer;
}
