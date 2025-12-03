//
// PacMan.h - IMPROVED VERSION
// Adds notifyDirectionChange() for Observer pattern compliance
//

#ifndef PACMAN_PACMAN_H
#define PACMAN_PACMAN_H
#include "EntityModel.h"

class PacMan: public EntityModel {
private:
  float speed;
  Direction desiredDirection;

  // Spawn position for respawning
  float spawnX, spawnY;

  bool isAlignedWithGrid() const;

public:
  PacMan(float x, float y)
      : EntityModel(x, y, 0.8f, 0.8f)
      , speed(10.0f)
      , desiredDirection(Direction::LEFT)
      , spawnX(x)
      , spawnY(y) {}

  ~PacMan() override = default;

  // Set desired direction (can be called before we can actually turn)
  void setDesiredDirection(Direction dir) {
    desiredDirection = dir;
  }

  Direction getDesiredDirection() const { return desiredDirection; }

  void executeTurn() {
    if (desiredDirection != Direction::NONE) {
      setDirection(desiredDirection);
      // Don't clear desiredDirection - let it persist
    }
  }

  void update(float deltaTime, bool canMove);

  float getSpeed() const { return speed; }
  void setSpeed(float s) { speed = s; }

  // Death and respawn methods
  void die() {
    notify(GameEvent::PACMAN_DIED);
  }

  void respawn() {
    setPosition(spawnX, spawnY);
    setDirection(Direction::LEFT);
    desiredDirection = Direction::NONE;
    notify(GameEvent::DIRECTION_CHANGED);
  }

  void setSpawnPosition(float x, float y) {
    spawnX = x;
    spawnY = y;
  }

  // ✅ NEW: Notify observers when direction changes (for sprite updates)
  // Should be called by World after successful turn
  void notifyDirectionChange() {
    notify(GameEvent::DIRECTION_CHANGED);
  }
};

#endif //PACMAN_PACMAN_H