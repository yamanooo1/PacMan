//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H
#include "EntityModel.h"

class World;

enum class GhostType {
  RANDOM,      // Locked direction
  AMBUSHER,    // Target ahead of PacMan
  CHASER       // Follow PacMan directly
};

enum class GhostState {
  WAITING,     // In center, not moving
  CHASING      // Hunting PacMan
};

class Ghost: public EntityModel {
private:
  GhostType type;
  GhostState state;
  float waitTimer;  // Countdown before leaving
  float speed;

public:
  Ghost(float x, float y, GhostType t, float waitTime)
      : EntityModel(x, y, 0.8f, 0.8f)  // ← Change size to 0.8 (same as PacMan)
      , type(t)
      , state(GhostState::WAITING)
      , waitTimer(waitTime)
      , speed(2.5f)
  {setDirection(Direction::UP);}

  ~Ghost() override = default;

  GhostType getType() const { return type; }
  GhostState getState() const { return state; }

  void onEaten() {
    notify(GameEvent::GHOST_EATEN);
  }

  // Update ghost (movement logic)
  void update(float deltaTime, World* world);
};


#endif //PACMAN_GHOST_H