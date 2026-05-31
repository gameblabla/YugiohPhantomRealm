#pragma once

#include <chrono>

namespace Utility
{   // frame and music timers, reporting elapsed time in seconds
    class Clock
    {
        std::chrono::steady_clock::time_point lastStartTime;
        float deltaTime;//time delta in seconds, but shouldn't hold more than a second
    public:
        bool initialize();
        bool shutdown();
        void start();
        void stop();
        void forceDelta();
        void newLoop();
        //time between the start of one frame to the next
        float lastLoopTime() const;
        static Clock clockInstance;
        static Clock musClockInstance;
    };
#define gameClock Utility::Clock::clockInstance
#define musicClock Utility::Clock::musClockInstance
#define gameDeltaTime Utility::Clock::clockInstance.lastLoopTime()
}
