#pragma once

// From YugiohGame.cpp
extern int exit_game;

namespace Game{

	class YugiohUnit
	{
	public:
		YugiohUnit* nextScreen;
		virtual void input(){}
		virtual void render(){}
		virtual void update(){}
		virtual void toThisScreen(){}
		//virtual ~YugiohUnit();
	};

}
