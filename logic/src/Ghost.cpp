//
// Created by yamanooo on 11/22/25.
//

#include "logic/Ghost.h"
#include "logic/World.h"     // Add this
#include "logic/PacMan.h"    // ADD THIS
#include "logic/Random.h"    // ADD THIS
#include <cmath>             // Add this
#include <iostream>          // Add this
#include <vector>            // ADD THIS



Ghost::Ghost(float x, float y, GhostType t, float waitTime)
    : EntityModel(x, y, 0.8f, 0.8f)
    , type(t)
    , state(GhostState::WAITING)
    , waitTimer(waitTime)
    , speed(2.5f)
    , lastDecisionGridX(-999)  // Initialize to impossible value
    , lastDecisionGridY(-999)
{
  setDirection(Direction::UP);
}

void Ghost::update(float deltaTime, World* world, PacMan* pacman) {
  if (!world) return;

  // Handle WAITING state
  if (state == GhostState::WAITING) {
    waitTimer -= deltaTime;
    if (waitTimer <= 0.0f) {
      state = GhostState::CHASING;
      std::cout << "[GHOST] Ghost leaving center! Type: "
                << static_cast<int>(type) << std::endl;
    }
    return;  // Don't move while waiting
  }

  // CHASING state - movement
  auto [x, y] = getPosition();
  Direction currentDir = getDirection();

  if (currentDir == Direction::NONE) return;  // No direction yet

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

  // NEW: Make AI decision when at center AND (at intersection OR wall ahead)
  bool needsDecision = atCenter && (gridX != lastDecisionGridX || gridY != lastDecisionGridY);
  bool canMakeDecision = isAtIntersection(gridX, gridY, world) || wallAhead;

  if (needsDecision && canMakeDecision) {
    Direction newDir = chooseNextDirection(gridX, gridY, world, pacman);
    if (newDir != Direction::NONE) {
      setDirection(newDir);
      lastDecisionGridX = gridX;
      lastDecisionGridY = gridY;
      currentDir = newDir;

      std::cout << "[AI] Ghost type " << static_cast<int>(type)
                << " at (" << gridX << "," << gridY << ") chose direction "
                << static_cast<int>(newDir) << std::endl;

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


std::vector<Direction> Ghost::getViableDirections(int gridX, int gridY, World* world) const {
    std::vector<Direction> viable;
    Direction current = getDirection();

    // Calculate opposite direction (we don't want to go backwards)
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
        // Skip opposite direction (don't go backwards) UNLESS it's the only option
        if (dir == opposite) continue;

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

    // If no viable directions (stuck at dead-end), MUST go backwards
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
  // Intersection = 3+ directions OR stuck at dead-end (need to turn around)
  return viable.size() >= 2;  // Changed from 3 to 2 to catch T-intersections
}

int Ghost::manhattanDistance(int x1, int y1, int x2, int y2) const {
  return std::abs(x1 - x2) + std::abs(y1 - y2);
}

Direction Ghost::chooseNextDirection(int gridX, int gridY, World* world, PacMan* pacman) {
    auto viable = getViableDirections(gridX, gridY, world);

    if (viable.empty()) {
        return getDirection();  // Keep current direction if stuck
    }

    if (viable.size() == 1) {
        return viable[0];  // Only one choice
    }

    // Multiple choices - use AI based on type
    Random& random = Random::getInstance();

    if (type == GhostType::RANDOM) {
        // With p=0.5, reconsider direction
        if (random.getBool()) {
            // Pick random viable direction
            int index = random.getInt(0, viable.size() - 1);
            return viable[index];
        } else {
            // Keep current direction if it's viable
            Direction current = getDirection();
            for (Direction dir : viable) {
                if (dir == current) return current;
            }
            // If current not viable, pick random
            int index = random.getInt(0, viable.size() - 1);
            return viable[index];
        }
    }

    // For AMBUSHER and CHASER, find target position
    int targetX, targetY;

    if (!pacman) {
        // Fallback if no pacman
        int index = random.getInt(0, viable.size() - 1);
        return viable[index];
    }

    auto [px, py] = pacman->getPosition();
    float centerX = px + pacman->getWidth() / 2.0f;
    float centerY = py + pacman->getHeight() / 2.0f;
    int pacmanGridX = static_cast<int>(std::floor(centerX));
    int pacmanGridY = static_cast<int>(std::floor(centerY));

    if (type == GhostType::CHASER) {
        // Target PacMan's current position
        targetX = pacmanGridX;
        targetY = pacmanGridY;
    } else {
        // AMBUSHER: Target ahead of PacMan
        Direction pacmanDir = pacman->getDirection();
        targetX = pacmanGridX;
        targetY = pacmanGridY;

        // Project 4 tiles ahead in PacMan's direction
        int lookAhead = 4;
        switch (pacmanDir) {
            case Direction::UP:    targetY -= lookAhead; break;
            case Direction::DOWN:  targetY += lookAhead; break;
            case Direction::LEFT:  targetX -= lookAhead; break;
            case Direction::RIGHT: targetX += lookAhead; break;
            case Direction::NONE: break;
        }
    }

    // Find direction that minimizes Manhattan distance
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

    // Break ties randomly
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