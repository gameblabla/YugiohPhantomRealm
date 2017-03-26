#pragma once
#ifdef USE_GLES
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif
#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <Game/Cards/CardRender.h>

#define YUG_BIG_CARD_EMPHASIS_ATK 1
#define YUG_BIG_CARD_EMPHASIS_DEF 2
#define YUG_BIG_CARD_EMPHASIS_BOTH 3
#define YUG_BIG_CARD_EMPHASIS_NONE 0


namespace Card{
	class BigCardRender : public Card::CardRender
	{
		static glm::vec3 nameStartPosition;
		static glm::vec3 attackStartPosition;
		static glm::vec3 defenseStartPosition;
		static glm::vec3 elementStartPosition;
		static glm::vec3 starchipsStartPosition;
		static glm::vec3 magicchipsStartPosition;

		static GLuint cardVertsVBO;
		static GLuint pictureVertsVBO;

		static GLuint backCardTBO;
		static GLuint monsterCardTBO;
		static GLuint magicCardTBO;
		static GLuint trapCardTBO;
#ifdef NOVA
		static GLuint frontCardVAO;
		static GLuint backCardVAO;
		static GLuint pictureVAO;
#endif
		GLuint returnCorrectFrameID();
		void continueStatCountUp();
		
	public:
		bool initialize();
		bool shutdown();
		bool startup();
		bool cleanup();
		void update();
		void render();

		float nameScaleX;
		int startAttack;
		int displayedAttack;
		int finalAttack;
		int startDefense;
		int displayedDefense;
		int finalDefense;
		float attackCountUpSpeed;
		float attackCountUpClock;//for atk rise animation
		bool raiseStat;							//from constellations
		int emphasisStat;
		bool hasStarted;
		void countStatUp(int amount, float duration);

		void emphasis(int stat);
	
	};

}
