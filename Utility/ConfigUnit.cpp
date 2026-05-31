#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <Utility/InputUnit.h>
#include <Utility/SoundUnit.h>
#include <Utility/ConfigUnit.h>
#include <SDL2/SDL.h>

namespace Utility{

	namespace{
		const char* CONFIG_VERSION = "SDL2_INPUT_V2";


		bool looksLikeLegacyWindowsConfig(const int* values){
			const int oldDefaults[YUG_NO_OF_KEYS] = {
				87, 83, 65, 68, 40, 39, 38, 37, 32, 82, 8, 76, 96
			};
			int exactMatches = 0;
			for(int i = 0; i < YUG_NO_OF_KEYS; ++i){
				if(values[i] == oldDefaults[i]){
					++exactMatches;
				}
			}
			if(exactMatches >= 4){
				return true;
			}

			// The original unversioned file stored Win32 virtual-key values. The
			// default shape is particularly distinctive: arrow-key Win32 values
			// for the face-button actions. Interpreting those numbers as SDL scancodes is
			// what made Enter appear to be the only working key: VK_DOWN == 40,
			// while SDL_SCANCODE_RETURN == 40.
			if(values[YUG_KEY_X] >= 37 && values[YUG_KEY_X] <= 40 &&
				values[YUG_KEY_O] >= 37 && values[YUG_KEY_O] <= 40){
				return true;
			}
			return false;
		}

		bool parseInt(const std::string& text, int* value){
			char* end = 0;
			long parsed = std::strtol(text.c_str(), &end, 10);
			if(end == text.c_str() || *end != '\0'){
				return false;
			}
			*value = static_cast<int>(parsed);
			return true;
		}

		bool validScancode(int scancode){
			return scancode >= 0 && scancode < SDL_NUM_SCANCODES;
		}
	}

	void ConfigUnit::startup(){}
	void ConfigUnit::cleanup(){}

	void ConfigUnit::saveConfig(){
		std::ofstream out("GameData/saves/options.txt");
		out<<CONFIG_VERSION;
		out<<"\n";
		for(int i = 0; i < YUG_NO_OF_KEYS; i++){
			out<<inputUnit.realKeyBindings[i];
			out<<"\n";
		}
		out<<soundUnit.soundOn; out<<"\n";
		out<<soundUnit.effectsOn; out<<"\n";
		out<<soundUnit.musicOn; out<<"\n";
	}

	void ConfigUnit::loadConfig(){
		std::ifstream in("GameData/saves/options.txt");
		if(!in.good()){
			return;
		}

		std::string firstToken;
		if(!(in>>firstToken)){
			return;
		}

		int values[YUG_NO_OF_KEYS];
		bool versioned = (firstToken == CONFIG_VERSION);

		if(versioned){
			for(int i = 0; i < YUG_NO_OF_KEYS; i++){
				if(!(in>>values[i])){
					return;
				}
			}
		}else{
			if(!parseInt(firstToken, &values[0])){
				return;
			}
			for(int i = 1; i < YUG_NO_OF_KEYS; i++){
				if(!(in>>values[i])){
					return;
				}
			}
		}

		bool legacyWindowsConfig = !versioned && looksLikeLegacyWindowsConfig(values);
		if(legacyWindowsConfig){
			// Do not reinterpret the original Win32 virtual-key defaults as SDL
			// scancodes. That maps VK_DOWN (40) onto SDL_SCANCODE_RETURN
			// (40), which is the observed "only Enter works" failure. Keep
			// the SDL2 defaults/aliases installed by InputUnit::initialize().
			std::cout<<"Config: ignoring legacy Win32 key bindings; using SDL2 defaults.\n";
		}else{
			for(int i = 0; i < YUG_NO_OF_KEYS; i++){
				if(validScancode(values[i])){
					inputUnit.realKeyBindings[i] = values[i];
				}
			}
		}

		in>>soundUnit.soundOn;
		in>>soundUnit.effectsOn;
		in>>soundUnit.musicOn;
	}

}
