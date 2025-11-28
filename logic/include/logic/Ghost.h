//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H
#include "EntityModel.h"
#include <vector>

class World;
class PacMan;

enum class GhostType {
  RANDOM,      // Locked direction
  AMBUSHER,    // Target ahead of PacMan
  CHASER       // Follow PacMan directly
};

enum class GhostState {
  WAITING,      // In center, not moving
  CHASING,      // Hunting PacMan
  FEAR
};

class Ghost: public EntityModel {
private:
  GhostType type;
  GhostState state;
  float waitTimer;  // Countdown before leaving
  float speed;
  float normalSpeed;    // NEW: Store normal speed
  float fearSpeed;      // NEW: Speed when in fear mode

  // NEW: Track where we last made a decision
  int lastDecisionGridX;
  int lastDecisionGridY;

  // NEW: AI helper methods
  std::vector<Direction> getViableDirections(int gridX, int gridY, World* world) const;
  bool isAtIntersection(int gridX, int gridY, World* world) const;
  Direction chooseNextDirection(int gridX, int gridY, World* world, PacMan* pacman);
  int manhattanDistance(int x1, int y1, int x2, int y2) const;

public:
  Ghost(float x, float y, GhostType t, float waitTime);

  ~Ghost() override = default;


  GhostType getType() const { return type; }
  GhostState getState() const { return state; }

  // NEW: Fear mode methods
  void enterFearMode();
  void exitFearMode();
  bool isInFearMode() const { return state == GhostState::FEAR; }

  void onEaten() {
    notify(GameEvent::GHOST_EATEN);
  }

  // Update ghost (movement logic)
  void update(float deltaTime, World* world, PacMan* pacman);  // Add PacMan parameter
};

#endif //PACMAN_GHOST_H