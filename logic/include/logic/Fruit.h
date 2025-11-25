//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_FRUIT_H
#define PACMAN_FRUIT_H
#include "Collectable.h"

class Fruit: public Collectable {
public:
  Fruit(float x, float y)
      : Collectable(x, y, 0.03f, 0.03f) {}  // Slightly bigger

  ~Fruit() override = default;
};


#endif //PACMAN_FRUIT_H