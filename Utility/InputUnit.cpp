#define WIN32_LEAN_AND_MEAN
//gameblabla FIX
//#include <Qt/qapplication.h>
#include <Game/YugiohUnit.h>
#include <Utility/InputUnit.h>
#include <Utility/StateUnit.h>
#include <Base/RenderUnit.h>
#include <iostream>
#include <Utility/SoundUnit.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <SDL2/SDL.h>         // std::chrono::seconds

namespace Utility{


	InputUnit InputUnit::inputUnitInstance;


	bool InputUnit::initialize(){
		resetAllKeys();
		takePlayerInputsOnly();
		Game::WaitUnit::startup();
		return true;
	}

	bool InputUnit::shutdown(){
		return true;
	}

	void InputUnit::readKeys()
	{
		setKeyBits = 0;
		
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                   exit_game = 1;
                    break;
                }  
			}
        }

		// SDL_PollEvent pumps the event queue; read the keyboard state after it.
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		
		if (state[SDL_SCANCODE_HOME] || state[SDL_SCANCODE_F4])
		{
			exit_game = 1;
		}
		
		int possibleAction = 1;
		for(int i = 0; i < YUG_NO_OF_KEYS; i++){
			if( state[realKeyBindings[i]]){
				setKeyBits |= possibleAction;
			}
			possibleAction <<= 1;
		}
	}

	void InputUnit::computerKeyPress(int key){
		int action = 1;
		action <<= key;//move bit to this keys position
		compSetKeyBits |= action;
	}

	bool InputUnit::isKeyActive(int key){
		int setKeys;
		if(playerInputAllowed)
			setKeys = (setKeyBits >> key);
		else
			setKeys = (compSetKeyBits >> key);
		return (setKeys%2);
	}

	void InputUnit::releaseAllComputerKeys(){
		compSetKeyBits = 0;
	}

	void InputUnit::callCurrentInputTaker(){
		//std::cout<<"Calling current Inputer/n";
		if(!isWaiting){
			currentInput->input();
		}else{
			continueWaiting();
		}
	}

	void InputUnit::setKeyBinding(int key, int newKeyBinding){

	}
	int InputUnit::listenForAnyKey(){
		int count = 0;
		int numKeys = 0;
		std::cout<<"Input: Hold down new key binding\n";
		while(count++ < 30){
			SDL_PumpEvents();
			const Uint8 *state = SDL_GetKeyboardState(&numKeys);
			std::this_thread::sleep_for (std::chrono::milliseconds(300));
			for (int scanCode = 0; scanCode < numKeys; scanCode++)
			{
				if(state[scanCode]){
					soundUnit.cursorSelect();
					std::cout<<"Input: New key binding obtained\n";
					return scanCode;
				}
			}
		}
		std::cout<<"Input: No new key binding obtained.\n";
		soundUnit.cursorSelectionFail();
		return -1;
	}
	void InputUnit::resetKey(int key){
		int res = listenForAnyKey();
		if(res==-1)return;
		for(int i = 0; i <YUG_NO_OF_KEYS; i++){
			if(res==realKeyBindings[i])return;
		}
		realKeyBindings[key] = res;
	}
	void InputUnit::resetAllKeys(){
		realKeyBindings[YUG_KEY_UP] = SDL_SCANCODE_UP;
		realKeyBindings[YUG_KEY_DOWN] = SDL_SCANCODE_DOWN;
		realKeyBindings[YUG_KEY_LEFT] = SDL_SCANCODE_LEFT;
		realKeyBindings[YUG_KEY_RIGHT] = SDL_SCANCODE_RIGHT;
		realKeyBindings[YUG_KEY_X] = SDL_SCANCODE_LCTRL;
		realKeyBindings[YUG_KEY_O] = SDL_SCANCODE_LALT;
		realKeyBindings[YUG_KEY_TRI] = SDL_SCANCODE_LSHIFT;
		realKeyBindings[YUG_KEY_SQU] = SDL_SCANCODE_SPACE;
		realKeyBindings[YUG_KEY_START] = SDL_SCANCODE_RETURN;
		realKeyBindings[YUG_KEY_SELECT] = SDL_SCANCODE_ESCAPE;
		realKeyBindings[YUG_KEY_R1] = SDL_SCANCODE_R;
		realKeyBindings[YUG_KEY_L1] = SDL_SCANCODE_L;
		realKeyBindings[YUG_KEY_SPECIAL] = SDL_SCANCODE_O;
	}

	void InputUnit::takePlayerInputsOnly(){
		playerInputAllowed = true;
		computerInputAllowed = false;
	}
	void InputUnit::takeComputerInputsOnly(){
		playerInputAllowed = false;
		computerInputAllowed = true;
	}
	void InputUnit::swapInputAllowances(){
		bool temp = playerInputAllowed;
		playerInputAllowed = computerInputAllowed;
		computerInputAllowed = temp;
	}
}
