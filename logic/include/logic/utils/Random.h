#ifndef PACMAN_RANDOM_H
#define PACMAN_RANDOM_H
#include <random>

namespace logic {

/**
 * @brief Pseudo-random number generator - singleton for game randomness
 *
 * Random provides consistent, high-quality random numbers using C++11 <random>.
 *
 * Design Pattern: Singleton
 * - Single instance ensures reproducible randomness (if seeded)
 * - Global access via getInstance()
 * - No construction/copying/moving allowed
 *
 * Generator: Mersenne Twister (std::mt19937)
 * - High-quality PRNG (2^19937 - 1 period)
 * - Fast generation
 * - Good statistical properties
 * - Seeded with std::random_device for non-determinism
 *
 * Use Cases in Game:
 * - Ghost AI: Random direction choices for unpredictability
 * - Ghost behavior: Coin flip for random vs. current direction
 * - Tie-breaking: Multiple equally good paths → pick one randomly
 *
 * Usage Examples:
 * ```cpp
 * Random& random = Random::getInstance();
 *
 * // Ghost random direction (0-3 maps to UP/DOWN/LEFT/RIGHT)
 * int dirIndex = random.getInt(0, 3);
 *
 * // Ghost coin flip (50/50 random vs continue)
 * if (random.getBool()) {
 *     // Random new direction
 * } else {
 *     // Continue current direction
 * }
 *
 * // Random spawn delay (2.0-5.0 seconds)
 * float delay = random.getFloat(2.0f, 5.0f);
 * ```
 *
 * Thread Safety: Not thread-safe (assumes single game thread)
 *
 * @see Ghost::chooseNextDirection() for AI randomness usage
 */
class Random {
private:
    std::mt19937 generator;  // Mersenne Twister PRNG

    /**
     * @brief Private constructor for singleton pattern
     *
     * Seeds generator with std::random_device for non-deterministic start.
     * Each game run will have different random sequence.
     *
     * Note: For testing/debugging, could modify to accept seed parameter.
     */
    Random() : generator(std::random_device{}()) {}

public:
    /**
     * @brief Get singleton instance
     *
     * Thread-safe in C++11+ (static initialization guaranteed).
     *
     * @return Reference to singleton instance
     */
    static Random& getInstance() {
        static Random instance;
        return instance;
    }

    // Delete copy/move operations (singleton enforcement)
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    /**
     * @brief Generate random float in range [min, max]
     *
     * Uses std::uniform_real_distribution for uniform distribution.
     *
     * Examples:
     * - getFloat(0.0f, 1.0f): Random probability/percentage
     * - getFloat(-1.0f, 1.0f): Random offset/noise
     * - getFloat(2.0f, 5.0f): Random delay/duration
     *
     * @param min Minimum value (inclusive)
     * @param max Maximum value (inclusive)
     * @return Random float in [min, max]
     */
    float getFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(generator);
    }

    /**
     * @brief Generate random integer in range [min, max]
     *
     * Uses std::uniform_int_distribution for uniform distribution.
     *
     * Examples:
     * - getInt(0, 3): Random direction index (4 directions)
     * - getInt(0, array.size()-1): Random array element
     * - getInt(1, 6): Simulated die roll
     *
     * @param min Minimum value (inclusive)
     * @param max Maximum value (inclusive)
     * @return Random integer in [min, max]
     */
    int getInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator);
    }

    /**
     * @brief Generate random boolean (50/50 coin flip)
     *
     * Equivalent to getInt(0, 1) == 1 but more semantic.
     *
     * Examples:
     * - Ghost decision: random vs. continue
     * - Random chance: if (getBool()) { ... }
     * - Tie breaking: pick first or second option
     *
     * @return true or false with equal probability
     */
    bool getBool() { return getInt(0, 1) == 1; }
};

} // namespace logic

#endif