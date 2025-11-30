//
// Ghost.cpp - Simple version with exit gates, NO backwards turns
//

#include "logic/Ghost.h"
#include "logic/World.h"
#include "logic/PacMan.h"
#include "logic/Random.h"
#include <cmath>
#include <iostream>
#include <vector>

Ghost::Ghost(float x, float y, GhostType t, float waitTime)
    : EntityModel(x, y, 0.8f, 0.8f)
    , type(t)
    , state(GhostState::WAITING)
    , waitTimer(waitTime)
    , speed(2.5f)
    , lastDecisionGridX(-999)
    , lastDecisionGridY(-999)
    , normalSpeed(2.5f)
    , fearSpeed(1.5f)
    , hasLeftSpawn(false)
{
  setDirection(Direction::UP);
}

void Ghost::update(float deltaTime, World* world, PacMan* pacman) {
  if (!world) return;

  // Handle WAITING state
  if (state == GhostState::WAITING) {
    waitTimer -= deltaTime;
    if (waitTimer <= 0.0f) {
      // Switch to EXITING state - navigate to exit gate
      state = GhostState::EXITING;

      auto [x, y] = getPosition();
      float centerX = x + getWidth() / 2.0f;
      float centerY = y + getHeight() / 2.0f;
      int gridX = static_cast<int>(std::floor(centerX));
      int gridY = static_cast<int>(std::floor(centerY));

      // Choose direction toward nearest exit gate
      Direction exitDir = chooseDirectionToExit(gridX, gridY, world);
      if (exitDir != Direction::NONE) {
        setDirection(exitDir);
        lastDecisionGridX = gridX;
        lastDecisionGridY = gridY;
      }

      std::cout << "[GHOST] Ghost type " << static_cast<int>(type)
                << " starting to exit spawn via 'w'" << std::endl;
    }
    return;  // Don't move while waiting
  }

  // Handle EXITING state
  if (state == GhostState::EXITING) {
    auto [x, y] = getPosition();
    float centerX = x + getWidth() / 2.0f;
    float centerY = y + getHeight() / 2.0f;
    int gridX = static_cast<int>(std::floor(centerX));
    int gridY = static_cast<int>(std::floor(centerY));

    // Check if we've reached an exit gate ('w')
    if (world->isExitPosition(gridX, gridY)) {
      state = GhostState::CHASING;
      hasLeftSpawn = true;

      std::cout << "[GHOST] Ghost type " << static_cast<int>(type)
                << " passed through exit 'w' at (" << gridX << "," << gridY
                << ") - now CHASING!" << std::endl;

      // Choose initial chase direction
      Direction chaseDir = chooseNextDirection(gridX, gridY, world, pacman);
      if (chaseDir != Direction::NONE) {
        setDirection(chaseDir);
        lastDecisionGridX = gridX;
        lastDecisionGridY = gridY;
      }
    }
    // Continue moving toward exit
  }

  // Movement logic for EXITING, CHASING or FEAR states
  auto [x, y] = getPosition();
  Direction currentDir = getDirection();

  if (currentDir == Direction::NONE) return;

  // Calculate center position
  float centerX = x + getWidth() / 2.0f;
  float centerY = y + getHeight() / 2.0f;

  int gridX = static_cast<int>(std::floor(centerX));
  int gridY = static_cast<int>(std::floor(centerY));

  // Check next grid cell for walls
  int nextGridX = gridX;
  int nextGridY = gridY;
  switch (currentDir) {
    case Direction::UP:    nextGridY--; break;
    case Direction::DOWN:  nextGridY++; break;
    case Direction::LEFT:  nextGridX--; break;
    case Direction::RIGHT: nextGridX++; break;
    case Direction::NONE: break;
  }
  bool wallAhead = world->hasWallInGridCell(nextGridX, nextGridY);

  // Check if at grid center (for making decisions)
  float gridCenterX = gridX + 0.5f;
  float gridCenterY = gridY + 0.5f;
  const float centerTolerance = 0.1f;
  bool atCenterX = std::abs(centerX - gridCenterX) < centerTolerance;
  bool atCenterY = std::abs(centerY - gridCenterY) < centerTolerance;
  bool atCenter = atCenterX && atCenterY;

  // Decision making
  bool needsDecision = atCenter && (gridX != lastDecisionGridX || gridY != lastDecisionGridY);
  bool canMakeDecision = isAtIntersection(gridX, gridY, world) || wallAhead;

  if (needsDecision && canMakeDecision) {
    Direction newDir = Direction::NONE;

    if (state == GhostState::EXITING) {
      // Navigate toward exit gate
      newDir = chooseDirectionToExit(gridX, gridY, world);
    } else {
      // Normal chase/fear logic
      newDir = chooseNextDirection(gridX, gridY, world, pacman);
    }

    if (newDir != Direction::NONE) {
      setDirection(newDir);
      lastDecisionGridX = gridX;
      lastDecisionGridY = gridY;
      currentDir = newDir;

      // Recalculate next grid with new direction
      nextGridX = gridX;
      nextGridY = gridY;
      switch (currentDir) {
        case Direction::UP:    nextGridY--; break;
        case Direction::DOWN:  nextGridY++; break;
        case Direction::LEFT:  nextGridX--; break;
        case Direction::RIGHT: nextGridX++; break;
        case Direction::NONE: break;
      }
      wallAhead = world->hasWallInGridCell(nextGridX, nextGridY);
    }
  }

  // Move if no wall ahead
  if (!wallAhead) {
    float moveDistance = speed * deltaTime;
    float newX = x;
    float newY = y;

    switch (currentDir) {
      case Direction::UP:    newY -= moveDistance; break;
      case Direction::DOWN:  newY += moveDistance; break;
      case Direction::LEFT:  newX -= moveDistance; break;
      case Direction::RIGHT: newX += moveDistance; break;
      case Direction::NONE: break;
    }

    setPosition(newX, newY);
  } else {
    // Wall ahead - move toward grid center then stop
    float targetCenterX = gridX + 0.5f;
    float targetCenterY = gridY + 0.5f;

    bool atTargetCenter = (std::abs(centerX - targetCenterX) < 0.01f &&
                           std::abs(centerY - targetCenterY) < 0.01f);

    if (!atTargetCenter) {
      float moveDistance = speed * deltaTime;
      float newCenterX = centerX;
      float newCenterY = centerY;

      switch (currentDir) {
        case Direction::UP:
          newCenterY = std::max(centerY - moveDistance, targetCenterY);
          break;
        case Direction::DOWN:
          newCenterY = std::min(centerY + moveDistance, targetCenterY);
          break;
        case Direction::LEFT:
          newCenterX = std::max(centerX - moveDistance, targetCenterX);
          break;
        case Direction::RIGHT:
          newCenterX = std::min(centerX + moveDistance, targetCenterX);
          break;
        case Direction::NONE:
          break;
      }

      float newX = newCenterX - getWidth() / 2.0f;
      float newY = newCenterY - getHeight() / 2.0f;
      setPosition(newX, newY);
    }
  }
}

// Navigate toward nearest exit gate ('w')
Direction Ghost::chooseDirectionToExit(int gridX, int gridY, World* world) const {
  auto exitPositions = world->getExitPositions();

  if (exitPositions.empty()) {
    std::cerr << "[ERROR] No exit positions ('w') found in map!" << std::endl;
    return Direction::UP;  // Fallback
  }

  // Find nearest exit
  int nearestDist = 999999;
  int targetX = exitPositions[0].first;
  int targetY = exitPositions[0].second;

  for (const auto& [ex, ey] : exitPositions) {
    int dist = manhattanDistance(gridX, gridY, ex, ey);
    if (dist < nearestDist) {
      nearestDist = dist;
      targetX = ex;
      targetY = ey;
    }
  }

  // Get viable directions (no backwards!)
  auto viable = getViableDirections(gridX, gridY, world);

  if (viable.empty()) {
    return getDirection();
  }

  if (viable.size() == 1) {
    return viable[0];
  }

  // Find direction that minimizes distance to exit
  std::vector<Direction> bestDirections;
  int bestDistance = 999999;

  for (Direction dir : viable) {
    int nextGridX = gridX;
    int nextGridY = gridY;

    switch (dir) {
      case Direction::UP:    nextGridY--; break;
      case Direction::DOWN:  nextGridY++; break;
      case Direction::LEFT:  nextGridX--; break;
      case Direction::RIGHT: nextGridX++; break;
      case Direction::NONE: break;
    }

    int distance = manhattanDistance(nextGridX, nextGridY, targetX, targetY);

    if (distance < bestDistance) {
      bestDistance = distance;
      bestDirections.clear();
      bestDirections.push_back(dir);
    } else if (distance == bestDistance) {
      bestDirections.push_back(dir);
    }
  }

  if (bestDirections.empty()) {
    return viable[0];
  }

  if (bestDirections.size() == 1) {
    return bestDirections[0];
  }

  // Break ties randomly
  Random& random = Random::getInstance();
  int index = random.getInt(0, bestDirections.size() - 1);
  return bestDirections[index];
}

// Get viable directions - BACKWARDS ALLOWED for smart ghosts
std::vector<Direction> Ghost::getViableDirections(int gridX, int gridY, World* world) const {
    std::vector<Direction> viable;
    Direction current = getDirection();

    // Calculate opposite direction
    Direction opposite = Direction::NONE;
    if (current == Direction::UP) opposite = Direction::DOWN;
    else if (current == Direction::DOWN) opposite = Direction::UP;
    else if (current == Direction::LEFT) opposite = Direction::RIGHT;
    else if (current == Direction::RIGHT) opposite = Direction::LEFT;

    // Check all 4 directions
    std::vector<Direction> allDirections = {
        Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT
    };

    for (Direction dir : allDirections) {
        // RANDOM ghost: skip backwards
        // CHASER/AMBUSHER: allow backwards if it's smart
        if (type == GhostType::RANDOM && dir == opposite) {
            continue;  // RANDOM never goes backwards
        }

        // Calculate next grid position
        int nextGridX = gridX;
        int nextGridY = gridY;
        switch (dir) {
            case Direction::UP:    nextGridY--; break;
            case Direction::DOWN:  nextGridY++; break;
            case Direction::LEFT:  nextGridX--; break;
            case Direction::RIGHT: nextGridX++; break;
            case Direction::NONE: break;
        }

        // If no wall, it's viable
        if (!world->hasWallInGridCell(nextGridX, nextGridY)) {
            viable.push_back(dir);
        }
    }

    // Dead-end fallback (shouldn't happen much now)
    if (viable.empty() && opposite != Direction::NONE) {
        int nextGridX = gridX;
        int nextGridY = gridY;
        switch (opposite) {
            case Direction::UP:    nextGridY--; break;
            case Direction::DOWN:  nextGridY++; break;
            case Direction::LEFT:  nextGridX--; break;
            case Direction::RIGHT: nextGridX++; break;
            case Direction::NONE: break;
        }
        if (!world->hasWallInGridCell(nextGridX, nextGridY)) {
            viable.push_back(opposite);
        }
    }

    return viable;
}

bool Ghost::isAtIntersection(int gridX, int gridY, World* world) const {
  auto viable = getViableDirections(gridX, gridY, world);
  return viable.size() >= 2;
}

int Ghost::manhattanDistance(int x1, int y1, int x2, int y2) const {
  return std::abs(x1 - x2) + std::abs(y1 - y2);
}

Direction Ghost::chooseNextDirection(int gridX, int gridY, World* world, PacMan* pacman) {
    auto viable = getViableDirections(gridX, gridY, world);

    if (viable.empty()) {
        return getDirection();
    }

    if (viable.size() == 1) {
        return viable[0];
    }

    Random& random = Random::getInstance();

    // ===== RANDOM GHOST =====
    if (type == GhostType::RANDOM) {
        Direction current = getDirection();

        bool currentViable = false;
        for (Direction dir : viable) {
            if (dir == current) {
                currentViable = true;
                break;
            }
        }

        if (random.getBool()) {
            int index = random.getInt(0, viable.size() - 1);
            return viable[index];
        } else {
            if (currentViable) {
                return current;
            } else {
                int index = random.getInt(0, viable.size() - 1);
                return viable[index];
            }
        }
    }

    // ===== AMBUSHER and CHASER GHOSTS =====

    if (!pacman) {
        int index = random.getInt(0, viable.size() - 1);
        return viable[index];
    }

    // Get Pac-Man's grid position
    auto [px, py] = pacman->getPosition();
    float centerX = px + pacman->getWidth() / 2.0f;
    float centerY = py + pacman->getHeight() / 2.0f;
    int pacmanGridX = static_cast<int>(std::floor(centerX));
    int pacmanGridY = static_cast<int>(std::floor(centerY));

    // Determine target
    int targetX, targetY;

    if (state == GhostState::FEAR) {
        targetX = pacmanGridX;
        targetY = pacmanGridY;
    } else if (type == GhostType::CHASER) {
        targetX = pacmanGridX;
        targetY = pacmanGridY;
    } else {  // AMBUSHER
        targetX = pacmanGridX;
        targetY = pacmanGridY;

        Direction pacmanDir = pacman->getDirection();
        int lookAhead = 4;

        switch (pacmanDir) {
            case Direction::UP:    targetY -= lookAhead; break;
            case Direction::DOWN:  targetY += lookAhead; break;
            case Direction::LEFT:  targetX -= lookAhead; break;
            case Direction::RIGHT: targetX += lookAhead; break;
            case Direction::NONE: break;
        }
    }

    bool shouldMaximize = (state == GhostState::FEAR);

    // Find best direction(s) - NO backwards considered
    std::vector<Direction> bestDirections;
    int bestDistance = shouldMaximize ? -1 : 999999;

    for (Direction dir : viable) {
        int nextGridX = gridX;
        int nextGridY = gridY;

        switch (dir) {
            case Direction::UP:    nextGridY--; break;
            case Direction::DOWN:  nextGridY++; break;
            case Direction::LEFT:  nextGridX--; break;
            case Direction::RIGHT: nextGridX++; break;
            case Direction::NONE: break;
        }

        int distance = manhattanDistance(nextGridX, nextGridY, targetX, targetY);

        if (shouldMaximize) {
            if (distance > bestDistance) {
                bestDistance = distance;
                bestDirections.clear();
                bestDirections.push_back(dir);
            } else if (distance == bestDistance) {
                bestDirections.push_back(dir);
            }
        } else {
            if (distance < bestDistance) {
                bestDistance = distance;
                bestDirections.clear();
                bestDirections.push_back(dir);
            } else if (distance == bestDistance) {
                bestDirections.push_back(dir);
            }
        }
    }

    if (bestDirections.empty()) {
        int index = random.getInt(0, viable.size() - 1);
        return viable[index];
    }

    if (bestDirections.size() == 1) {
        return bestDirections[0];
    }

    int index = random.getInt(0, bestDirections.size() - 1);
    return bestDirections[index];
}

void Ghost::enterFearMode() {
  if (state == GhostState::WAITING || state == GhostState::EXITING) return;

  state = GhostState::FEAR;
  speed = fearSpeed;

  Direction current = getDirection();
  Direction opposite = Direction::NONE;

  if (current == Direction::UP) opposite = Direction::DOWN;
  else if (current == Direction::DOWN) opposite = Direction::UP;
  else if (current == Direction::LEFT) opposite = Direction::RIGHT;
  else if (current == Direction::RIGHT) opposite = Direction::LEFT;

  if (opposite != Direction::NONE) {
    setDirection(opposite);
  }

  lastDecisionGridX = -999;
  lastDecisionGridY = -999;

  std::cout << "[FEAR] Ghost type " << static_cast<int>(type)
            << " entered FEAR mode!" << std::endl;
}

void Ghost::exitFearMode() {
  if (state != GhostState::FEAR) return;

  state = GhostState::CHASING;
  speed = normalSpeed;

  lastDecisionGridX = -999;
  lastDecisionGridY = -999;

  std::cout << "[FEAR] Ghost type " << static_cast<int>(type)
            << " exited FEAR mode!" << std::endl;
}

void Ghost::resetSpawnFlag() {
  hasLeftSpawn = false;
  state = GhostState::EXITING;  // Go back to exiting state when respawning
}