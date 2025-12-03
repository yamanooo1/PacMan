//
// Ghost.cpp - FIXED: Store and reset original wait time
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
    , originalWaitTime(waitTime)  // ✅ NEW: Store original for reset
    , speed(2.5f)
    , lastDecisionGridX(-999)
    , lastDecisionGridY(-999)
    , normalSpeed(2.5f)
    , fearSpeed(1.5f)
    , hasLeftSpawn(false)
{
  setDirection(Direction::UP);
}

bool Ghost::isInSpawnArea(int gridX, int gridY) const {
  return (gridX >= 7 && gridX <= 11 && gridY >= 8 && gridY <= 10);
}

void Ghost::update(float deltaTime, World* world, PacMan* pacman) {
  if (!world) return;

  // WAITING state
  if (state == GhostState::WAITING) {
    waitTimer -= deltaTime;
    if (waitTimer <= 0.0f) {
      state = GhostState::EXITING;

      auto [x, y] = getPosition();
      float centerX = x + getWidth() / 2.0f;
      float centerY = y + getHeight() / 2.0f;
      int gridX = static_cast<int>(std::floor(centerX));
      int gridY = static_cast<int>(std::floor(centerY));

      Direction exitDir = chooseDirectionToExit(gridX, gridY, world);
      if (exitDir != Direction::NONE) {
        setDirection(exitDir);
        lastDecisionGridX = gridX;
        lastDecisionGridY = gridY;
      }

      std::cout << "[GHOST] Type " << static_cast<int>(type)
                << " starting to exit via 'w'" << std::endl;
    }
    return;
  }

  // EXITING state
  if (state == GhostState::EXITING) {
    auto [x, y] = getPosition();
    float centerX = x + getWidth() / 2.0f;
    float centerY = y + getHeight() / 2.0f;
    int gridX = static_cast<int>(std::floor(centerX));
    int gridY = static_cast<int>(std::floor(centerY));

    if (world->isExitPosition(gridX, gridY)) {
      state = GhostState::CHASING;
      hasLeftSpawn = true;

      std::cout << "[GHOST] Type " << static_cast<int>(type)
                << " passed through 'w' at (" << gridX << "," << gridY
                << ") - now CHASING!" << std::endl;

      Direction chaseDir = chooseNextDirection(gridX, gridY, world, pacman);
      if (chaseDir != Direction::NONE) {
        setDirection(chaseDir);
        lastDecisionGridX = gridX;
        lastDecisionGridY = gridY;
      }
    }
  }

  // Movement
  auto [x, y] = getPosition();
  Direction currentDir = getDirection();

  if (currentDir == Direction::NONE) return;

  float centerX = x + getWidth() / 2.0f;
  float centerY = y + getHeight() / 2.0f;
  int gridX = static_cast<int>(std::floor(centerX));
  int gridY = static_cast<int>(std::floor(centerY));

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

  float gridCenterX = gridX + 0.5f;
  float gridCenterY = gridY + 0.5f;
  const float centerTolerance = 0.1f;
  bool atCenterX = std::abs(centerX - gridCenterX) < centerTolerance;
  bool atCenterY = std::abs(centerY - gridCenterY) < centerTolerance;
  bool atCenter = atCenterX && atCenterY;

  bool needsDecision = atCenter && (gridX != lastDecisionGridX || gridY != lastDecisionGridY);
  bool canMakeDecision = isAtIntersection(gridX, gridY, world) || wallAhead;

  if (needsDecision && canMakeDecision) {
    Direction newDir = Direction::NONE;

    if (state == GhostState::EXITING) {
      newDir = chooseDirectionToExit(gridX, gridY, world);
    } else {
      newDir = chooseNextDirection(gridX, gridY, world, pacman);
    }

    if (newDir != Direction::NONE) {
      setDirection(newDir);
      lastDecisionGridX = gridX;
      lastDecisionGridY = gridY;
      currentDir = newDir;

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

Direction Ghost::chooseDirectionToExit(int gridX, int gridY, World* world) const {
  auto exitPositions = world->getExitPositions();

  if (exitPositions.empty()) {
    std::cerr << "[ERROR] No exit positions!" << std::endl;
    return Direction::UP;
  }

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

  auto viable = getViableDirections(gridX, gridY, world);

  if (viable.empty()) return getDirection();
  if (viable.size() == 1) return viable[0];

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

  if (bestDirections.empty()) return viable[0];
  if (bestDirections.size() == 1) return bestDirections[0];

  Random& random = Random::getInstance();
  int index = random.getInt(0, bestDirections.size() - 1);
  return bestDirections[index];
}

std::vector<Direction> Ghost::getViableDirections(int gridX, int gridY, World* world) const {
    std::vector<Direction> viable;
    Direction current = getDirection();

    Direction opposite = Direction::NONE;
    if (current == Direction::UP) opposite = Direction::DOWN;
    else if (current == Direction::DOWN) opposite = Direction::UP;
    else if (current == Direction::LEFT) opposite = Direction::RIGHT;
    else if (current == Direction::RIGHT) opposite = Direction::LEFT;

    std::vector<Direction> allDirections = {
        Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT
    };

    for (Direction dir : allDirections) {
        if (dir == opposite) continue;

        int nextGridX = gridX;
        int nextGridY = gridY;
        switch (dir) {
            case Direction::UP:    nextGridY--; break;
            case Direction::DOWN:  nextGridY++; break;
            case Direction::LEFT:  nextGridX--; break;
            case Direction::RIGHT: nextGridX++; break;
            case Direction::NONE: break;
        }

        if (world->hasWallInGridCell(nextGridX, nextGridY)) {
            continue;
        }

        if (hasLeftSpawn && isInSpawnArea(nextGridX, nextGridY)) {
            continue;
        }

        viable.push_back(dir);
    }

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

        bool hasWall = world->hasWallInGridCell(nextGridX, nextGridY);
        bool wouldEnterSpawn = hasLeftSpawn && isInSpawnArea(nextGridX, nextGridY);

        if (!hasWall && !wouldEnterSpawn) {
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

    if (viable.empty()) return getDirection();
    if (viable.size() == 1) return viable[0];

    Random& random = Random::getInstance();

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

    if (!pacman) {
        int index = random.getInt(0, viable.size() - 1);
        return viable[index];
    }

    auto [px, py] = pacman->getPosition();
    float centerX = px + pacman->getWidth() / 2.0f;
    float centerY = py + pacman->getHeight() / 2.0f;
    int pacmanGridX = static_cast<int>(std::floor(centerX));
    int pacmanGridY = static_cast<int>(std::floor(centerY));

    int targetX, targetY;

    if (state == GhostState::FEAR) {
        targetX = pacmanGridX;
        targetY = pacmanGridY;
    } else if (type == GhostType::CHASER) {
        targetX = pacmanGridX;
        targetY = pacmanGridY;
    } else {
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
}

void Ghost::exitFearMode() {
  if (state != GhostState::FEAR) return;

  state = GhostState::CHASING;
  speed = normalSpeed;

  lastDecisionGridX = -999;
  lastDecisionGridY = -999;
}