//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_STOPWATCH_H
#define PACMAN_STOPWATCH_H

#include <chrono>

class Stopwatch {
private:
  using Clock = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<Clock>;

  TimePoint startTime;    // When stopwatch was created
  TimePoint lastTime;     // Last recorded time for deltaTime
  float deltaTime;        // Time difference between frames

  // Private constructor
  Stopwatch() : deltaTime(0.0f) {
    startTime = Clock::now();
    lastTime = startTime;
  }

public:
  // Singleton access
  static Stopwatch& getInstance() {
    static Stopwatch instance;
    return instance;
  }

  // Delete copy and move operations
  Stopwatch(const Stopwatch&) = delete;
  Stopwatch& operator=(const Stopwatch&) = delete;
  Stopwatch(Stopwatch&&) = delete;
  Stopwatch& operator=(Stopwatch&&) = delete;

  // Updates lastTime and deltaTime
  void tick() {
    TimePoint currentTime = Clock::now();
    std::chrono::duration<float> elapsed = currentTime - lastTime;
    deltaTime = elapsed.count();
    lastTime = currentTime;
  }

  // Get time since last frame
  float getDeltaTime() const {
    return deltaTime;
  }

  // Get total elapsed time since stopwatch started
  float getElapsedTime() const {
    TimePoint currentTime = Clock::now();
    std::chrono::duration<float> elapsed = currentTime - startTime;
    return elapsed.count();
  }
};

#endif //PACMAN_STOPWATCH_H