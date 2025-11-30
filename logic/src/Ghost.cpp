//
// Created by yamanooo on 11/22/25.
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

      // NEW: Choose intelligent initial direction when leaving center
      auto [x, y] = getPosition();
      float centerX = x + getWidth() / 2.0f;
      float centerY = y + getHeight() / 2.0f;
      int gridX = static_cast<int>(std::floor(centerX));
      int gridY = static_cast<int>(std::floor(centerY));

      // Get an intelligent direction based on AI type
      Direction smartDirection = chooseNextDirection(gridX, gridY, world, pacman);
      if (smartDirection != Direction::NONE) {
        setDirection(smartDirection);
        lastDecisionGridX = gridX;
        lastDecisionGridY = gridY;
      }

      std::cout << "[GHOST] Ghost leaving center! Type: "
                << static_cast<int>(type)
                << " Initial direction: " << static_cast<int>(smartDirection) << std::endl;
    }
    return;  // Don't move while waiting
  }

  // Movement logic for CHASING or FEAR states
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

  // Make AI decision when at center AND (at intersection OR wall ahead) AND haven't decided here yet
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
  // Intersection = 2+ viable directions (includes T-intersections and 4-way)
  return viable.size() >= 2;
}

int Ghost::manhattanDistance(int x1, int y1, int x2, int y2) const {
  return std::abs(x1 - x2) + std::abs(y1 - y2);
}

Direction Ghost::chooseNextDirection(int gridX, int gridY, World* world, PacMan* pacman) {
    auto viable = getViableDirections(gridX, gridY, world);

    if (viable.empty()) {
        return getDirection();  // Keep current direction if stuck (shouldn't happen)
    }

    if (viable.size() == 1) {
        return viable[0];  // Only one choice - no AI needed
    }

    // Multiple choices - use AI based on type and state
    Random& random = Random::getInstance();

    // ===== RANDOM GHOST =====
    // Assignment: "With probability p=0.5, ghost will lock to a random direction"
    // This means: 50% chance to pick new random, 50% chance to keep current (if viable)
    if (type == GhostType::RANDOM) {
        Direction current = getDirection();

        // Check if current direction is in viable options
        bool currentViable = false;
        for (Direction dir : viable) {
            if (dir == current) {
                currentViable = true;
                break;
            }
        }

        // With p=0.5, reconsider direction (pick random)
        // With p=0.5, keep current direction (if viable)
        if (random.getBool()) {
            // Reconsider: pick a random viable direction
            int index = random.getInt(0, viable.size() - 1);
            std::cout << "[AI] RANDOM ghost reconsidering (p=0.5) - picked random direction" << std::endl;
            return viable[index];
        } else {
            // Don't reconsider: keep current if viable, else pick random
            if (currentViable) {
                std::cout << "[AI] RANDOM ghost keeping current direction (p=0.5)" << std::endl;
                return current;
            } else {
                // Current not viable (shouldn't happen at intersection), pick random
                int index = random.getInt(0, viable.size() - 1);
                return viable[index];
            }
        }
    }

    // ===== AMBUSHER and CHASER GHOSTS =====
    // These ghosts use Manhattan distance to a target position

    if (!pacman) {
        // Fallback if no pacman (shouldn't happen)
        int index = random.getInt(0, viable.size() - 1);
        return viable[index];
    }

    // Get Pac-Man's grid position
    auto [px, py] = pacman->getPosition();
    float centerX = px + pacman->getWidth() / 2.0f;
    float centerY = py + pacman->getHeight() / 2.0f;
    int pacmanGridX = static_cast<int>(std::floor(centerX));
    int pacmanGridY = static_cast<int>(std::floor(centerY));

    // Determine target position based on ghost type
    int targetX, targetY;

    if (type == GhostType::CHASER) {
        // CHASER: Target Pac-Man's current position directly
        targetX = pacmanGridX;
        targetY = pacmanGridY;
    } else {  // GhostType::AMBUSHER
        // AMBUSHER: Target 4 tiles ahead of Pac-Man in the direction he's facing
        // Assignment: "move in the direction that minimizes the Manhattan distance
        // to the locations "in front of" Pac-Man"
        targetX = pacmanGridX;
        targetY = pacmanGridY;

        Direction pacmanDir = pacman->getDirection();
        int lookAhead = 4;  // Look 4 tiles ahead

        switch (pacmanDir) {
            case Direction::UP:    targetY -= lookAhead; break;
            case Direction::DOWN:  targetY += lookAhead; break;
            case Direction::LEFT:  targetX -= lookAhead; break;
            case Direction::RIGHT: targetX += lookAhead; break;
            case Direction::NONE:
                // If Pac-Man isn't moving, just target his current position
                break;
        }
    }

    // In FEAR mode, ghosts flee (maximize distance from target)
    // In CHASING mode, ghosts chase (minimize distance to target)
    bool shouldMaximize = (state == GhostState::FEAR);

    // Find the best direction(s) based on Manhattan distance
    std::vector<Direction> bestDirections;
    int bestDistance = shouldMaximize ? -1 : 999999;

    for (Direction dir : viable) {
        // Calculate where we'd be after moving in this direction
        int nextGridX = gridX;
        int nextGridY = gridY;

        switch (dir) {
            case Direction::UP:    nextGridY--; break;
            case Direction::DOWN:  nextGridY++; break;
            case Direction::LEFT:  nextGridX--; break;
            case Direction::RIGHT: nextGridX++; break;
            case Direction::NONE: break;
        }

        // Calculate Manhattan distance from that position to target
        int distance = manhattanDistance(nextGridX, nextGridY, targetX, targetY);

        if (shouldMaximize) {
            // FEAR mode: we want to be as FAR as possible from target
            if (distance > bestDistance) {
                bestDistance = distance;
                bestDirections.clear();
                bestDirections.push_back(dir);
            } else if (distance == bestDistance) {
                bestDirections.push_back(dir);  // Tie
            }
        } else {
            // CHASING mode: we want to be as CLOSE as possible to target
            if (distance < bestDistance) {
                bestDistance = distance;
                bestDirections.clear();
                bestDirections.push_back(dir);
            } else if (distance == bestDistance) {
                bestDirections.push_back(dir);  // Tie
            }
        }
    }

    // Break ties randomly (assignment requirement: "Ties between the best actions are broken at random")
    if (bestDirections.empty()) {
        // Shouldn't happen, but fallback
        int index = random.getInt(0, viable.size() - 1);
        return viable[index];
    }

    if (bestDirections.size() == 1) {
        return bestDirections[0];  // Clear winner
    }

    // Multiple directions tied for best - pick randomly
    int index = random.getInt(0, bestDirections.size() - 1);
    std::cout << "[AI] " << (state == GhostState::FEAR ? "FEAR" : "CHASE")
              << " mode tie-break: chose 1 of " << bestDirections.size() << " options" << std::endl;
    return bestDirections[index];
}

void Ghost::enterFearMode() {
  if (state == GhostState::WAITING) return;  // Don't affect waiting ghosts

  state = GhostState::FEAR;
  speed = fearSpeed;  // Slow down

  // Reverse direction immediately (assignment: "reverses their direction")
  Direction current = getDirection();
  Direction opposite = Direction::NONE;

  if (current == Direction::UP) opposite = Direction::DOWN;
  else if (current == Direction::DOWN) opposite = Direction::UP;
  else if (current == Direction::LEFT) opposite = Direction::RIGHT;
  else if (current == Direction::RIGHT) opposite = Direction::LEFT;

  if (opposite != Direction::NONE) {
    setDirection(opposite);
  }

  // Reset decision tracking so ghost can make new decision immediately
  lastDecisionGridX = -999;
  lastDecisionGridY = -999;

  std::cout << "[FEAR] Ghost type " << static_cast<int>(type)
            << " entered FEAR mode - reversed direction!" << std::endl;
}

void Ghost::exitFearMode() {
  if (state != GhostState::FEAR) return;

  state = GhostState::CHASING;
  speed = normalSpeed;  // Back to normal speed

  // Reset decision tracking so ghost can reconsider direction
  lastDecisionGridX = -999;
  lastDecisionGridY = -999;

  std::cout << "[FEAR] Ghost type " << static_cast<int>(type)
            << " exited FEAR mode - back to chasing!" << std::endl;
}