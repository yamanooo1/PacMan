#ifndef PACMAN_PACMAN_H
#define PACMAN_PACMAN_H
#include "EntityModel.h"

class PacMan: public EntityModel {
private:
  float speed;
  Direction desiredDirection;
  float spawnX, spawnY;

  bool isAlignedWithGrid() const;

public:
  PacMan(float x, float y);
  ~PacMan() override = default;

  void setDesiredDirection(Direction dir);
  Direction getDesiredDirection() const { return desiredDirection; }

  void executeTurn();
  void update(float deltaTime, bool canMove);

  float getSpeed() const { return speed; }
  void setSpeed(float s) { speed = s; }

  void die();
  void respawn();
  void setSpawnPosition(float x, float y);
  void notifyDirectionChange();
  void hitWall();
};

#endif