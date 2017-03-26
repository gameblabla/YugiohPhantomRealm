#pragma once
#ifdef USE_GLES
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <Game/Cards/CardRender.h>

namespace Card{
	class SmallCardRender : public Card::CardRender
	{
		static glm::vec3 atkStartPosition;
		static glm::vec3 defStartPosition;
		static glm::vec3 magicStartPosition;

		static GLuint cardVertsVBO;
		static GLuint pictureVertsVBO;

		static GLuint backCardTBO;
		static GLuint monsterCardTBO;
		static GLuint magicCardTBO;
		static GLuint trapCardTBO;
		static GLuint hiddenTBO;
		static GLuint hiddenCardTBO;
#ifndef NOVA
		static GLuint frontCardVAO;
		static GLuint backCardVAO;
		static GLuint pictureVAO;
#endif
		GLuint returnCorrectFrameID();
	public:

		bool initialize();
		bool shutdown();
		bool startup();
		bool cleanup();
		void update();
		void render();
	
	};

}
