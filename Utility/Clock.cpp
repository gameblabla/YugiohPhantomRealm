#include <Utility/Clock.h>
#include <Utility/ErrorHandler.h>
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <chrono>

namespace Utility{

	Clock Clock::clockInstance;
	Clock Clock::musClockInstance;
	
	std::chrono::high_resolution_clock timer;
	std::chrono::time_point<std::chrono::system_clock> start_timer;

	bool Clock::initialize()
	{
#ifdef _WIN32
		//get how many 'tick' in a second =/= clock speed, but should be constant
		if(! QueryPerformanceFrequency(&timeFrequency))
			return false; //if getting 'ticks' failed, quit
		//try to get performance once to see that it works
		return QueryPerformanceCounter(&lastStartTime);
#else
		return 1;
#endif
	}

	void Clock::start()
	{
#ifdef _WIN32
		QueryPerformanceCounter(&lastStartTime);
#else
		start_timer = timer.now();
#endif
	}

	void Clock::stop(){
#ifdef _WIN32
		LARGE_INTEGER delta;
		LARGE_INTEGER thisTime;
		QueryPerformanceCounter(&thisTime);
		//get time difference between new and old frame in 'ticks'
		delta.QuadPart = thisTime.QuadPart - lastStartTime.QuadPart;
		//get difference in seconds
		deltaTime = ((float)delta.QuadPart) / timeFrequency.QuadPart;
		deltaLastLap.QuadPart = thisTime.QuadPart;
		if(deltaTime == 0.0f){
			errorHandler.printError("Clock.cpp: Delta time == 0 warning");
		}
#else
		auto timer_stop = timer.now();
		// According to original source code above, it needs the time difference in seconds but milliseconds works better ?
		//deltaTime = std::chrono::duration_cast<std::chrono::seconds>(timer_stop - start_timer).count();
		deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(timer_stop - start_timer).count();
#endif
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
