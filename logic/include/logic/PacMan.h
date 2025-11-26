//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_PACMAN_H
#define PACMAN_PACMAN_H
#include "EntityModel.h"

class PacMan: public EntityModel {
private:
  float speed;
  Direction desiredDirection;
  bool isAlignedWithGrid() const;
public:
  PacMan(float x, float y) : EntityModel(x, y, 0.8f, 0.8f), speed(3.0f), desiredDirection(Direction::NONE) {};

  // NEW: Set desired direction (can be called before we can actually turn)
  void setDesiredDirection(Direction dir) {
    desiredDirection = dir;
  }


  ~PacMan() override = default;

  void update(float deltaTime, bool canMove);

  float getSpeed() const { return speed; }
  void setSpeed(float s) { speed = s; }
};


#endif //PACMAN_PACMAN_H