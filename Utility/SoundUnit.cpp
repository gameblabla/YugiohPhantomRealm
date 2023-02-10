//#include <irrKlang.h>
#include <Utility/SoundUnit.h>
#include <Utility/Clock.h>
#include <Utility/ErrorHandler.h>
//using namespace irrklang;
//gameblabla FIX

#include <SDL2/SDL_mixer.h>
#define MAX_SFX 16
Mix_Chunk* sfx_id[MAX_SFX];
Mix_Music* music;

#define CARD_SWIPE2 0
#define CARD_SWIPE3 1
#define GAMEBEEP 2
#define CURSOR_UNSUCESS 3
#define CURSOR_SELECT 4
#define CURSOR_CANCEL 5
#define CURSOR_MOVE 6


namespace Utility{

	SoundUnit SoundUnit::soundInstance;

	bool SoundUnit::initialize(){
		Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024);
		Mix_AllocateChannels(MAX_SFX);
		music = Mix_LoadMUS("GameData/sounds/music/warmupMusic.wav");
		Mix_PlayMusic(music, -1);
		
		printf("Music Play\n");
		
		sfx_id[CARD_SWIPE2] = Mix_LoadWAV("GameData/sounds/card/cardSwipe2.wav");
		sfx_id[CARD_SWIPE3] = Mix_LoadWAV("GameData/sounds/card/cardSwipe3.wav");
		sfx_id[GAMEBEEP] = Mix_LoadWAV("GameData/sounds/menu/gameBeep.wav");
		sfx_id[CURSOR_UNSUCESS] = Mix_LoadWAV("GameData/sounds/menu/cursorUnsuccess.wav");
		sfx_id[CURSOR_MOVE] = Mix_LoadWAV("GameData/sounds/menu/cursorMove.wav");
		sfx_id[CURSOR_SELECT] = Mix_LoadWAV("GameData/sounds/menu/cursorSelect4.wav");
		sfx_id[CURSOR_CANCEL] = Mix_LoadWAV("GameData/sounds/menu/cursorCancel.wav");

		soundOn = true;
		effectsOn = true;
		musicOn = true;
		musicCounting = false;
		
		/*musicSource = createIrrKlangDevice();
		if(!musicSource){
			errorHandler.printError("Sound Unit: irrKlang music engine failed to load.");
			return false;
		}
		soundOn = true;
		effectsOn = true;
		musicOn = true;
		musicCounting = false;*/
		//musicSource->play2D("GameData/sounds/music/warmupMusic.wav", true);
		
		return true;
	}
	bool SoundUnit::shutdown(){
		/*if(musicSource)
			musicSource->drop();*/
		return true;
	}

	void SoundUnit::update(){
		/*if(musicCounting){
			musicClock.newLoop();
			timeLeft -= musicClock.lastLoopTime();
			if(timeLeft<=0)
				moveToNextTrack();
		}*/
	}
	void SoundUnit::playLeadinTrack(const char* track, const char* followup, float length){
		if(!musicOn) return;
		musicCounting = true;
		nextTrack = followup;
		timeLeft = length;
		musicClock.newLoop();
		playOnce(track);
	}
	void SoundUnit::moveToNextTrack(){
		musicCounting = false;
		if(!musicOn) return;
		playLoop(nextTrack.c_str());
		nextTrack = "";
	}

	void SoundUnit::playOnce(const char* filename){
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		
		music = Mix_LoadMUS(filename);
		Mix_PlayMusic(music, 0);
			//musicSource->play2D(filename);
	}
	void SoundUnit::playLoop(const char* filename){
		if(soundOn && musicOn){
			Mix_HaltMusic();
			Mix_FreeMusic(music);
			
			music = Mix_LoadMUS(filename);
			Mix_PlayMusic(music, -1);
			//musicSource->play2D(filename, true);
		}
	}
	void SoundUnit::stopAll(){
		Mix_HaltMusic();
		//musicSource->removeAllSoundSources();
	}
	void SoundUnit::stopThisSound(const char* filename){
		//musicSource->removeSoundSource(filename);
	}

	void SoundUnit::cardSwoosh(){
		if(soundOn && effectsOn)
		{
			if (sfx_id[CARD_SWIPE2])
			{
				Mix_FreeChunk(sfx_id[CARD_SWIPE2]);
				sfx_id[CARD_SWIPE2] = NULL;
			}
			sfx_id[CARD_SWIPE2] = Mix_LoadWAV("GameData/sounds/card/cardSwipe2.wav");
			//soundUnit.playOnce("GameData/sounds/card/cardSwipe2.wav");
		}
	}
	void SoundUnit::cardSwivel(){
		if(soundOn && effectsOn)
			Mix_PlayChannel(-1, sfx_id[CARD_SWIPE3], 0) ;
	}
	void SoundUnit::cursorMove(){
		if(soundOn && effectsOn)
			Mix_PlayChannel(-1, sfx_id[GAMEBEEP], 0) ;
	}
	void SoundUnit::cursorSelectionSuccess(){}
	void SoundUnit::cursorSelectionFail(){
		if(soundOn && effectsOn)
			Mix_PlayChannel(-1, sfx_id[CURSOR_UNSUCESS], 0) ;
	}

	void SoundUnit::cursorSelect(){
		if(soundOn && effectsOn)
			Mix_PlayChannel(-1, sfx_id[CURSOR_SELECT], 0) ;
	}
	void SoundUnit::cursorCancel(){
		if(soundOn && effectsOn)
			Mix_PlayChannel(-1, sfx_id[CURSOR_CANCEL], 0) ;
	}








}
