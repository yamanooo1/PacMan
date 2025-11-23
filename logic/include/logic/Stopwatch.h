//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_STOPWATCH_H
#define PACMAN_STOPWATCH_H


#include <chrono>

class Stopwatch {
private:

    using Clock = std::chrono::high_resolution_clock; // most accurate clock
    using TimePoint = std::chrono::time_point<Clock>; // represents point in time

    TimePoint lastTime; // Stores the last recorded time
    float deltaTime; // Time Difference between frames

    //Private constructor, initialize deltaTime to 0
    Stopwatch() : deltaTime(0.0f) {
        lastTime = Clock::now(); // Get current time
    }

public:
    // The ONLY way to access the singleton.
    static Stopwatch& getInstance() {
        static Stopwatch instance;
        return instance; //returns a reference to a StopWatch == you can access the original object == more efficient.
    }

    // Delete copy and move operations
    Stopwatch(const Stopwatch&) = delete;
    Stopwatch& operator=(const Stopwatch&) = delete;
    Stopwatch(Stopwatch&&) = delete;
    Stopwatch& operator=(Stopwatch&&) = delete;

    //updates lastTime and deltTime
    void tick() {
        TimePoint currentTime = Clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        deltaTime = elapsed.count();
        lastTime = currentTime;
    }

    float getDeltaTime() const {
        return deltaTime;
    }
};

#endif //PACMAN_STOPWATCH_H