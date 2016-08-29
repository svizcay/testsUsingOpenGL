#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

class Timer {
    public:
        Timer();
        void update();

        float deltaTime;        // since last frame
        long long nrFrames;

    private:
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::high_resolution_clock::time_point now;
        float previousTime;
        float elapsedTime;      // since the beginning

        float fps;
};

#endif /* TIMER_HPP */
