#ifdef USE_GLES
#include <GLES2/gl2.h>
#endif

#include <cstdint>
#include <cmath>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <SDL2/SDL.h>

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

void computeMatricesFromInputs(){
	static uint64_t lastTime = SDL_GetPerformanceCounter();
	uint64_t currentTime = SDL_GetPerformanceCounter();
	float deltaTime = static_cast<float>(static_cast<double>(currentTime - lastTime)
		/ static_cast<double>(SDL_GetPerformanceFrequency()));
	lastTime = currentTime;

	SDL_PumpEvents();

	// SDL relative mouse mode gives movement deltas without manually warping the cursor.
	static bool relativeMouseEnabled = false;
	if(!relativeMouseEnabled){
		if(SDL_SetRelativeMouseMode(SDL_TRUE) == 0){
			relativeMouseEnabled = true;
		}
	}

	int mouseDeltaX = 0;
	int mouseDeltaY = 0;
	SDL_GetRelativeMouseState(&mouseDeltaX, &mouseDeltaY);

	// Compute new orientation
	horizontalAngle += mouseSpeed * static_cast<float>(-mouseDeltaX);
	verticalAngle   += mouseSpeed * static_cast<float>(-mouseDeltaY);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		std::cos(verticalAngle) * std::sin(horizontalAngle), 
		std::sin(verticalAngle),
		std::cos(verticalAngle) * std::cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		std::sin(horizontalAngle - 3.14f/2.0f), 
		0,
		std::cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

	// Move forward
	if (keyboardState[SDL_SCANCODE_UP] || keyboardState[SDL_SCANCODE_W]){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (keyboardState[SDL_SCANCODE_DOWN] || keyboardState[SDL_SCANCODE_S]){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (keyboardState[SDL_SCANCODE_RIGHT] || keyboardState[SDL_SCANCODE_D]){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (keyboardState[SDL_SCANCODE_LEFT] || keyboardState[SDL_SCANCODE_A]){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV - 5;
	
	// Projection matrix : 45 degree Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );
}
