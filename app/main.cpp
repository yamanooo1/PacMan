#include <iostream>
#include "representation/Game.h"

int main() {
  Game game;

  if (!game.initialize("../../resources/map/map1.txt")) {
    std::cerr << "Failed to initialize game!" << std::endl;
    return 1;
  }

  game.run();

  return 0;
}