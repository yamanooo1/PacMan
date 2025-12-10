#ifndef PACMAN_ENTITY_H
#define PACMAN_ENTITY_H
#include "Subject.h"
#include <utility>

enum class Direction {
  UP, DOWN, LEFT, RIGHT, NONE
};

class EntityModel: public Subject {
private:
  float x, y;
  const float width, height;
  Direction direction;
  bool dead;

public:
  EntityModel(float posX, float posY, float w, float h);
  ~EntityModel() override = default;

  std::pair<float,float> getPosition() const { return {x, y}; }
  void setPosition(float newX, float newY) { x = newX; y = newY; }

  float getWidth() const { return width; }
  float getHeight() const { return height; }

  Direction getDirection() const { return direction; }
  void setDirection(Direction dir) { direction = dir; }

  bool isDead() const { return dead; }
  void markDead() { dead = true; }

  virtual void onCollisionWithPacMan();
};

#endif