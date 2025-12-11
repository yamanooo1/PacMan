#ifndef PACMAN_RANDOM_H
#define PACMAN_RANDOM_H
#include <random>

class Random {
private:
  std::mt19937 generator;

  Random() : generator(std::random_device{}()) {}

public:
  static Random& getInstance() {
    static Random instance;
    return instance;
  }

  Random(const Random&) = delete;
  Random& operator=(const Random&) = delete;

  float getFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(generator);
  }

  int getInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(generator);
  }

  bool getBool() {
    return getInt(0, 1) == 1;
  }
};

#endif