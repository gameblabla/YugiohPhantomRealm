#define WIN32_LEAN_AND_MEAN
//gameblabla FIX
//#include <Qt/qapplication.h>
#include <Game/YugiohUnit.h>
#include <Utility/InputUnit.h>
#include <Utility/StateUnit.h>
#include <Base/RenderUnit.h>
#include <iostream>
#include <Utility/SoundUnit.h>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>

namespace Utility{

	InputUnit InputUnit::inputUnitInstance;

	namespace{
		bool scancodeIsPressed(const Uint8* state, int numKeys, int scancode){
			return state != 0 && scancode >= 0 && scancode < numKeys && state[scancode] != 0;
		}

		bool anyScancodePressed(const Uint8* state, int numKeys, const int* scancodes, int count){
			for(int i = 0; i < count; ++i){
				if(scancodeIsPressed(state, numKeys, scancodes[i])){
					return true;
				}
			}
			return false;
		}

		bool defaultAliasPressed(int action, const Uint8* state, int numKeys){
			switch(action){
			case YUG_KEY_UP:{
				const int keys[] = { SDL_SCANCODE_UP, SDL_SCANCODE_W };
				return anyScancodePressed(state, numKeys, keys, 2);
			}
			case YUG_KEY_DOWN:{
				const int keys[] = { SDL_SCANCODE_DOWN, SDL_SCANCODE_S };
				return anyScancodePressed(state, numKeys, keys, 2);
			}
			case YUG_KEY_LEFT:{
				const int keys[] = { SDL_SCANCODE_LEFT, SDL_SCANCODE_A };
				return anyScancodePressed(state, numKeys, keys, 2);
			}
			case YUG_KEY_RIGHT:{
				const int keys[] = { SDL_SCANCODE_RIGHT, SDL_SCANCODE_D };
				return anyScancodePressed(state, numKeys, keys, 2);
			}
			case YUG_KEY_X:{
				const int keys[] = {
					SDL_SCANCODE_RETURN, SDL_SCANCODE_KP_ENTER,
					SDL_SCANCODE_SPACE, SDL_SCANCODE_Z,
					SDL_SCANCODE_LCTRL, SDL_SCANCODE_RCTRL
				};
				return anyScancodePressed(state, numKeys, keys, 6);
			}
			case YUG_KEY_O:{
				const int keys[] = {
					SDL_SCANCODE_ESCAPE, SDL_SCANCODE_BACKSPACE,
					SDL_SCANCODE_X, SDL_SCANCODE_LALT, SDL_SCANCODE_RALT
				};
				return anyScancodePressed(state, numKeys, keys, 5);
			}
			case YUG_KEY_TRI:{
				const int keys[] = { SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT };
				return anyScancodePressed(state, numKeys, keys, 2);
			}
			case YUG_KEY_SQU:{
				const int keys[] = { SDL_SCANCODE_C };
				return anyScancodePressed(state, numKeys, keys, 1);
			}
			case YUG_KEY_START:{
				const int keys[] = { SDL_SCANCODE_RETURN, SDL_SCANCODE_KP_ENTER, SDL_SCANCODE_SPACE };
				return anyScancodePressed(state, numKeys, keys, 3);
			}
			case YUG_KEY_SELECT:{
				const int keys[] = { SDL_SCANCODE_ESCAPE, SDL_SCANCODE_BACKSPACE };
				return anyScancodePressed(state, numKeys, keys, 2);
			}
			case YUG_KEY_R1:{
				const int keys[] = { SDL_SCANCODE_R };
				return anyScancodePressed(state, numKeys, keys, 1);
			}
			case YUG_KEY_L1:{
				const int keys[] = { SDL_SCANCODE_L };
				return anyScancodePressed(state, numKeys, keys, 1);
			}
			case YUG_KEY_SPECIAL:{
				const int keys[] = { SDL_SCANCODE_O, SDL_SCANCODE_KP_0 };
				return anyScancodePressed(state, numKeys, keys, 2);
			}
			default:
				return false;
			}
		}
	}

	bool InputUnit::initialize(){
		setKeyBits = 0;
		compSetKeyBits = 0;
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

		int numKeys = 0;
		const Uint8 *state = SDL_GetKeyboardState(&numKeys);
		
		if (scancodeIsPressed(state, numKeys, SDL_SCANCODE_HOME) ||
			scancodeIsPressed(state, numKeys, SDL_SCANCODE_F4))
		{
			exit_game = 1;
		}
		
		int possibleAction = 1;
		for(int i = 0; i < YUG_NO_OF_KEYS; i++){
			if(scancodeIsPressed(state, numKeys, realKeyBindings[i]) ||
				defaultAliasPressed(i, state, numKeys)){
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
		if(key < 0 || key >= YUG_NO_OF_KEYS || newKeyBinding < 0 || newKeyBinding >= SDL_NUM_SCANCODES){
			return;
		}
		realKeyBindings[key] = newKeyBinding;
	}

	int InputUnit::listenForAnyKey(){
		std::cout<<"Input: Press new key binding\n";
		SDL_Event event;
		int elapsed = 0;
		while(elapsed < 9000){
			while(SDL_PollEvent(&event)){
				if(event.type == SDL_QUIT){
					exit_game = 1;
					return -1;
				}
				if(event.type == SDL_KEYDOWN && event.key.keysym.scancode >= 0 &&
					event.key.keysym.scancode < SDL_NUM_SCANCODES){
					soundUnit.cursorSelect();
					std::cout<<"Input: New key binding obtained\n";
					return event.key.keysym.scancode;
				}
			}
			std::this_thread::sleep_for (std::chrono::milliseconds(10));
			elapsed += 10;
		}
		std::cout<<"Input: No new key binding obtained.\n";
		soundUnit.cursorSelectionFail();
		return -1;
	}
	void InputUnit::resetKey(int key){
		int res = listenForAnyKey();
		if(res==-1)return;
		for(int i = 0; i <YUG_NO_OF_KEYS; i++){
			if(i != key && res==realKeyBindings[i])return;
		}
		realKeyBindings[key] = res;
	}
	void InputUnit::resetAllKeys(){
		realKeyBindings[YUG_KEY_UP] = SDL_SCANCODE_UP;
		realKeyBindings[YUG_KEY_DOWN] = SDL_SCANCODE_DOWN;
		realKeyBindings[YUG_KEY_LEFT] = SDL_SCANCODE_LEFT;
		realKeyBindings[YUG_KEY_RIGHT] = SDL_SCANCODE_RIGHT;
		realKeyBindings[YUG_KEY_X] = SDL_SCANCODE_SPACE;
		realKeyBindings[YUG_KEY_O] = SDL_SCANCODE_ESCAPE;
		realKeyBindings[YUG_KEY_TRI] = SDL_SCANCODE_LSHIFT;
		realKeyBindings[YUG_KEY_SQU] = SDL_SCANCODE_C;
		realKeyBindings[YUG_KEY_START] = SDL_SCANCODE_RETURN;
		realKeyBindings[YUG_KEY_SELECT] = SDL_SCANCODE_BACKSPACE;
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
