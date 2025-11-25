//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_COIN_H
#define PACMAN_COIN_H
#include "Collectable.h"

class Coin: public Collectable {

public:
  Coin(float x, float y)
      : Collectable(x, y, 0.02f, 0.02f) {}

  ~Coin() override = default;

  void onCollected() override{
    notify(GameEvent::COIN_COLLECTED);
  }
};


#endif //PACMAN_COIN_H