//
// Created by yamanooo on 11/22/25.
//

#include "logic/Ghost.h"
#include "logic/World.h"     // Add this
#include <cmath>             // Add this
#include <iostream>          // Add this

void Ghost::update(float deltaTime, World* world) {
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

  // Check next grid cell
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

  // Move if no wall
  if (!wallAhead) {
    // No wall - move normally
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
