#include "timer.hpp"

#include <iostream>

Timer::Timer() {
    start = std::chrono::high_resolution_clock::now();
    previousTime = 0.0f;
    nrFrames = 0;
}

void Timer::update() {
    now = std::chrono::high_resolution_clock::now();
    elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(now-start).count();
    deltaTime = elapsedTime - previousTime;
    previousTime = elapsedTime;
    nrFrames++;
    fps = nrFrames / elapsedTime;
}

