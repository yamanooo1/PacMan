#include <iostream>
#include "representation/Game.h"

int main() {
  Game game;

  if (!game.initialize()) {
    std::cerr << "Failed to initialize game!" << std::endl;
    return 1;
  }

  game.run();

  return 0;
}

//todo:: fix de scaling probleem van window screen
//todo:: menu muziek moet spelen
//todo:: Na victory: keuze 1. "want to continue?" space dan blijft de score tellen. 2."no" dan krijg jij de score naam screen
//todo:: Na dood: first death animation dan keuze.
//todo:: when level cleared the sound effects should stop