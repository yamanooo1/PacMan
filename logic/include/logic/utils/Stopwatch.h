#ifndef PACMAN_STOPWATCH_H
#define PACMAN_STOPWATCH_H
#include <chrono>

namespace logic {

class Stopwatch {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint startTime;
    TimePoint lastTime;
    float deltaTime;

    Stopwatch() : deltaTime(0.0f) {
        startTime = Clock::now();
        lastTime = startTime;
    }

public:
    static Stopwatch& getInstance() {
        static Stopwatch instance;
        return instance;
    }

    Stopwatch(const Stopwatch&) = delete;
    Stopwatch& operator=(const Stopwatch&) = delete;
    Stopwatch(Stopwatch&&) = delete;
    Stopwatch& operator=(Stopwatch&&) = delete;

    void tick() {
        TimePoint currentTime = Clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        deltaTime = elapsed.count();
        lastTime = currentTime;
    }

    [[nodiscard]] float getDeltaTime() const { return deltaTime; }

    [[nodiscard]] float getElapsedTime() const {
        TimePoint currentTime = Clock::now();
        std::chrono::duration<float> elapsed = currentTime - startTime;
        return elapsed.count();
    }
};

} // namespace logic

#endif