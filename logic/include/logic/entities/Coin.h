#ifndef PACMAN_COIN_H
#define PACMAN_COIN_H
#include "Collectable.h"

class Coin: public Collectable {
public:
  Coin(float x, float y);
  ~Coin() override = default;

  void onCollected() override;
};

#endif