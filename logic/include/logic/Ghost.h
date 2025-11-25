//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H
#include "EntityModel.h"

class Ghost: public EntityModel {
public:
  Ghost(float x, float y) : EntityModel(x, y, 0.05f, 0.05f) {};

  ~Ghost() override = default;

  void onEaten() {
    notify(GameEvent::GHOST_EATEN);
  }
};


#endif //PACMAN_GHOST_H