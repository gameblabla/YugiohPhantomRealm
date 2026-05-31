#include <Utility/Clock.h>
#include <Utility/ErrorHandler.h>
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <chrono>

namespace Utility{

	Clock Clock::clockInstance;
	Clock Clock::musClockInstance;
	
	bool Clock::initialize()
	{
		deltaTime = 0.0f;
		lastStartTime = std::chrono::steady_clock::now();
		return true;
	}

	void Clock::start()
	{
		lastStartTime = std::chrono::steady_clock::now();
	}

	void Clock::stop(){
		auto timer_stop = std::chrono::steady_clock::now();
		deltaTime = std::chrono::duration<float>(timer_stop - lastStartTime).count();
		if(deltaTime == 0.0f){
			errorHandler.printError("Clock.cpp: Delta time == 0 warning");
		}
	}

	bool Clock::shutdown(){//for now, nothing to close
		return true;
	}
	void Clock::newLoop(){
		stop();
		start();
	}
	
	float Clock::lastLoopTime() const{
		//printf("deltaTime %f\n", deltaTime);
		return deltaTime;
	}

	void Clock::forceDelta(){
		for(int i = 0; i <100; i++){}
	}

}
