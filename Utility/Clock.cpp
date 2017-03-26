#include <Utility/Clock.h>
#include <Utility/ErrorHandler.h>
#include <time.h>

#define BILLION 1E9
struct timespec requestStart, requestEnd;
struct timespec now;   

namespace Utility{

	Clock Clock::clockInstance;
	Clock Clock::musClockInstance;

	bool Clock::initialize()
	{
		  clock_gettime(CLOCK_MONOTONIC, &now);   
		  timeFrequency = now.tv_sec + now.tv_nsec / 1000000000.0;
		//get how many 'tick' in a second =/= clock speed, but should be constant
		/*if(! QueryPerformanceFrequency(&timeFrequency))
			return false;//if getting 'ticks' failed, quit
		//try to get performance once to see that it works
		return QueryPerformanceCounter(&lastStartTime);*/
		return 1;
		
	}

	void Clock::start()
	{
		//QueryPerformanceCounter(&lastStartTime);
		clock_gettime(CLOCK_MONOTONIC_RAW, &requestStart);
	}
	void Clock::stop(){
		LARGE_INTEGER delta;
		LARGE_INTEGER thisTime;
		
		clock_gettime(CLOCK_MONOTONIC_RAW, &requestEnd);
		delta = &requestEnd - &requestStart;
		deltaTime = ((float)delta / timeFrequency);
		deltaLastLap = requestEnd;
		if(deltaTime == 0.0f)
		{
			errorHandler.printError("Clock.cpp: Delta time == 0 warning");
		}
		
		//QueryPerformanceCounter(&thisTime);
		//get time difference between new and old frame in 'ticks'
		
		/*delta.QuadPart= thisTime.QuadPart - lastStartTime.QuadPart;
		//get difference in seconds
		deltaTime = ((float)delta.QuadPart) / timeFrequency.QuadPart;
		deltaLastLap.QuadPart = thisTime.QuadPart;
		if(deltaTime == 0.0f){
			errorHandler.printError("Clock.cpp: Delta time == 0 warning");
		}*/
	}

	bool Clock::shutdown(){//for now, nothing to close
		return true;
	}
	void Clock::newLoop(){
		stop();
		start();
		

	}
	float Clock::lastLoopTime() const{
		return deltaTime;
	}

	void Clock::forceDelta(){
		for(int i = 0; i <100; i++){}
	}

}
