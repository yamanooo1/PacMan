#ifndef PACMAN_COLLECTABLES_H
#define PACMAN_COLLECTABLES_H
#include "../core/EntityModel.h"

class Collectable: public EntityModel {
public:
  Collectable(float x, float y, float width, float height);
  ~Collectable() override = default;

  virtual void onCollected() = 0;

  void onCollisionWithPacMan() override;
};

#endif