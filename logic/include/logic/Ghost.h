//
// Ghost.h - FIXED: Wait timers properly reset on respawn
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
  EXITING,      // Navigating to 'w' exit
  CHASING,      // Hunting PacMan
  FEAR
};

class Ghost: public EntityModel {
private:
  GhostType type;
  GhostState state;
  float waitTimer;          // Current countdown timer
  float originalWaitTime;   // ✅ NEW: Original wait time for reset
  float speed;
  float normalSpeed;
  float fearSpeed;

  int lastDecisionGridX;
  int lastDecisionGridY;

  bool hasLeftSpawn;

  // AI helper methods
  std::vector<Direction> getViableDirections(int gridX, int gridY, World* world) const;
  bool isAtIntersection(int gridX, int gridY, World* world) const;
  Direction chooseNextDirection(int gridX, int gridY, World* world, PacMan* pacman);
  Direction chooseDirectionToExit(int gridX, int gridY, World* world) const;
  int manhattanDistance(int x1, int y1, int x2, int y2) const;
  bool isInSpawnArea(int gridX, int gridY) const;

public:
  Ghost(float x, float y, GhostType t, float waitTime);
  ~Ghost() override = default;

  GhostType getType() const { return type; }
  GhostState getState() const { return state; }

  void enterFearMode();
  void exitFearMode();
  bool isInFearMode() const { return state == GhostState::FEAR; }

  // ✅ FIXED: Now properly resets wait timer
  void resetSpawnFlag() {
    hasLeftSpawn = false;
    state = GhostState::WAITING;  // ← Changed to WAITING (not EXITING)
    waitTimer = originalWaitTime; // ← NEW: Reset timer to original value
  }

  void onEaten() {
    notify(GameEvent::GHOST_EATEN);
  }

  void update(float deltaTime, World* world, PacMan* pacman);
};

#endif //PACMAN_GHOST_H