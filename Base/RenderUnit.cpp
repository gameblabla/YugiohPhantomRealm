#ifdef USE_GLES
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif
#include <SDL2/SDL.h>

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

SDL_Window* gWindow;
SDL_Renderer* sdlrenderer;
SDL_GLContext gContext;

int frameCount;
float timeCount;
bool RenderUnit::initialize(){
	int success = true;
	frameCount = 0;
	timeCount = 0.0f;
	
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "Yugioh", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, YUG_DEFAULT_SCREEN_WIDTH, YUG_DEFAULT_SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
     
		sdlrenderer = SDL_CreateRenderer(gWindow, -1, 0);
     
		SDL_RenderSetLogicalSize(sdlrenderer, YUG_DEFAULT_SCREEN_WIDTH, YUG_DEFAULT_SCREEN_HEIGHT);
			
		// Make sure background is black
		SDL_SetRenderDrawColor(sdlrenderer, 255, 255, 255, 255);
		SDL_RenderClear(sdlrenderer);
		SDL_RenderPresent(sdlrenderer);
		
		gContext = SDL_GL_CreateContext(gWindow);
		
#ifdef USE_GLES
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif

		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );

    }
    
    // It has to be there otherwise it will crash hard when trying to call OpenGL functions right after render.initiliaze()
#ifndef USE_GLES
	printf("Initiliaze GLEW\n");
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
	
	//resize(YUG_DEFAULT_SCREEN_WIDTH,YUG_DEFAULT_SCREEN_HEIGHT);
	//move(YUG_WINDOW_ORIGIN_POINT);
	oldRenderer = NULL;
	currentRenderer = NULL;
	//show();
	return success;
}

void RenderUnit::initializeGL(){
}

bool RenderUnit::shutdown(){
	return true;
}

void RenderUnit::render(){
	SDL_GL_SwapWindow(gWindow);
	//glDraw();
}

void RenderUnit::paintEvent(){
	//blocks QT's auto/uncontrollable repainting
}

void RenderUnit::paintGL(){
	frameCount ++;
	timeCount += gameClock.lastLoopTime();
	
	glViewport(0,	0,	YUG_DEFAULT_SCREEN_WIDTH,	YUG_DEFAULT_SCREEN_HEIGHT );
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
