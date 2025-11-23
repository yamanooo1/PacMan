//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_RANDOM_H
#define PACMAN_RANDOM_H
#include <random>

class Random {
private:
  // Mersenne Twister random number generator - produces high-quality random numbers
  std::mt19937 generator;

  // Private constructor - seeds the generator with a random value from hardware
  // std::random_device{}() creates a random device and gets a random seed from it
  Random() : generator(std::random_device{}()) {}

public:
  // Returns the single instance of Random (Singleton pattern)
  // static local variable is created once and persists for the program's lifetime
  static Random& getInstance() {
    static Random instance;
    return instance;
  }

  // Delete copy constructor - prevents copying the singleton
  Random(const Random&) = delete;

  // Delete assignment operator - prevents assignment of the singleton
  Random& operator=(const Random&) = delete;

  // Returns a random float between min (inclusive) and max (exclusive)
  // Uses uniform distribution - all values have equal probability
  float getFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(generator);
  }

  // Returns a random integer between min and max (both inclusive)
  // Example: getInt(1, 6) for dice roll returns 1, 2, 3, 4, 5, or 6
  int getInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(generator);
  }

  // Returns true or false with 50% probability each
  // Useful for coin flips or binary decisions
  bool getBool() {
    return getInt(0, 1) == 1;
  }
};


#endif //PACMAN_RANDOM_H