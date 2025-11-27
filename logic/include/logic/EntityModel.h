//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_ENTITY_H
#define PACMAN_ENTITY_H
#include "Subject.h"

enum class Direction {
  UP, DOWN, LEFT, RIGHT, NONE
};
class EntityModel: public Subject {

private:

  // x,y
  float x, y;

  //width, height
  const float width, height;

  Direction direction;

  bool dead;

public:

  EntityModel(float posX, float posY, float w, float h)
    : x(posX),y(posY), width(w), height(h), direction(Direction::NONE), dead(false){}

  ~EntityModel() override = default;

  std::pair<float,float> getPosition() const{ return {x,y};}
  void setPosition(float newX, float newY){ x = newX; y = newY;}

  float getWidth() const { return width; }
  float getHeight() const { return height; }

  Direction getDirection() const { return direction; }
  void setDirection(Direction dir) { this->direction = dir; }

  bool isDead() const { return dead; }
  void markDead() { dead = true; }

  // NEW: Polymorphic collision handler - override in derived classes
  virtual void onCollisionWithPacMan() {}
};


#endif //PACMAN_ENTITY_H