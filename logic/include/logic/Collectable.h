//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_COLLECTABLES_H
#define PACMAN_COLLECTABLES_H
#include "EntityModel.h"

class Collectable: public EntityModel {
public:

  Collectable(float x, float y, float width, float height) : EntityModel(x, y, width, height) {};

  ~Collectable() override = default;

  virtual void onCollected() = 0;

  void onCollisionWithPacMan() override {
    onCollected();  // Notify observers (Score)
    markDead();     // Mark for removal
  }
};


#endif //PACMAN_COLLECTABLES_H