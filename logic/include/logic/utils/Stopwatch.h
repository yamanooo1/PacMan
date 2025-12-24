#ifndef PACMAN_STOPWATCH_H
#define PACMAN_STOPWATCH_H
#include <chrono>

namespace logic {

/**
 * @brief High-resolution game timer - singleton for consistent time tracking
 *
 * Stopwatch provides precise timing for game loop and animations using
 * C++ chrono high_resolution_clock.
 *
 * Design Pattern: Singleton
 * - Single instance ensures consistent time across all systems
 * - Global access via getInstance()
 * - No construction/copying/moving allowed
 *
 * Time Measurements:
 * 1. Delta Time: Time between frames (for movement/animation)
 * 2. Elapsed Time: Total time since game start (for absolute timestamps)
 *
 * Usage Pattern:
 * ```cpp
 * // Game loop (typically in Game::run()):
 * Stopwatch& stopwatch = Stopwatch::getInstance();
 *
 * while (running) {
 *     stopwatch.tick();  // Update delta time
 *     float deltaTime = stopwatch.getDeltaTime();
 *
 *     world->update(deltaTime);  // Use for movement
 *     factory->updateAll();      // Use for animations
 * }
 * ```
 *
 * Delta Time Example (60 FPS target):
 * - Frame 1: deltaTime ≈ 0.016s (16ms)
 * - Frame 2: deltaTime ≈ 0.017s (17ms, slight variance)
 * - Frame 3: deltaTime ≈ 0.016s (16ms)
 * - Average: 60 FPS
 *
 * Elapsed Time Example (Score system):
 * ```cpp
 * float currentTime = stopwatch.getElapsedTime();
 * float timeSince = currentTime - lastEventTime;  // Measure intervals
 * ```
 *
 * Thread Safety: Not thread-safe (assumes single game thread)
 *
 * @see Score::update() for elapsed time usage
 * @see Ghost::update() for delta time usage
 */
class Stopwatch {
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint startTime;  // Game start time (for elapsed time)
    TimePoint lastTime;   // Last tick time (for delta time)
    float deltaTime;      // Time since last tick (seconds)

    /**
     * @brief Private constructor for singleton pattern
     *
     * Initializes all time points to current time.
     * First tick() call will have deltaTime = 0.
     */
    Stopwatch() : deltaTime(0.0f) {
        startTime = Clock::now();
        lastTime = startTime;
    }

public:
    /**
     * @brief Get singleton instance
     *
     * Thread-safe in C++11+ (static initialization guaranteed).
     *
     * @return Reference to singleton instance
     */
    static Stopwatch& getInstance() {
        static Stopwatch instance;
        return instance;
    }

    // Delete copy/move operations (singleton enforcement)
    Stopwatch(const Stopwatch&) = delete;
    Stopwatch& operator=(const Stopwatch&) = delete;
    Stopwatch(Stopwatch&&) = delete;
    Stopwatch& operator=(Stopwatch&&) = delete;

    /**
     * @brief Update delta time (call once per frame)
     *
     * Calculates time elapsed since last tick() call.
     * Should be called at the start of each game loop iteration.
     *
     * Algorithm:
     * ```
     * currentTime = now()
     * deltaTime = currentTime - lastTime
     * lastTime = currentTime
     * ```
     *
     * Example (60 FPS):
     * - Frame duration: ~16.67ms
     * - deltaTime: ~0.0167 seconds
     * - Used for: movement = speed * deltaTime
     */
    void tick() {
        TimePoint currentTime = Clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        deltaTime = elapsed.count();
        lastTime = currentTime;
    }

    /**
     * @brief Get time since last tick (frame time)
     *
     * Used for frame-rate-independent movement and animation:
     * - Movement: position += velocity * deltaTime
     * - Animation: timer += deltaTime
     * - Physics: Euler integration step size
     *
     * @return Time since last tick in seconds (typically 0.008 - 0.033 for 30-120 FPS)
     */
    [[nodiscard]] float getDeltaTime() const { return deltaTime; }

    /**
     * @brief Get total time since stopwatch creation (game start)
     *
     * Used for absolute timestamps and interval calculations:
     * - Score decay timing
     * - Animation phase calculations
     * - Event timing (last coin collection, etc.)
     *
     * @return Elapsed time in seconds since game start
     */
    [[nodiscard]] float getElapsedTime() const {
        TimePoint currentTime = Clock::now();
        std::chrono::duration<float> elapsed = currentTime - startTime;
        return elapsed.count();
    }
};

} // namespace logic

#endif