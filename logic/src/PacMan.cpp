//
// Created by yamanooo on 11/22/25.
//

#include "logic/PacMan.h"
#include <cmath>

bool PacMan::isAlignedWithGrid() const {
  auto [x, y] = getPosition();
  const float tolerance = 0.1f;

  // Check if we're close to a grid cell center
  float fractX = x - std::floor(x);
  float fractY = y - std::floor(y);

  return (std::abs(fractX - 0.5f) < tolerance ||
          std::abs(fractX) < tolerance ||
          std::abs(fractX - 1.0f) < tolerance) &&
         (std::abs(fractY - 0.5f) < tolerance ||
          std::abs(fractY) < tolerance ||
          std::abs(fractY - 1.0f) < tolerance);
}
void PacMan::update(float deltaTime, bool canMove) {
  auto [x, y] = getPosition();

  // Try to change to desired direction if we're aligned
  if (desiredDirection != Direction::NONE &&
      desiredDirection != getDirection()) {
    // Can we turn in the desired direction?
    // For now, just accept the turn (we'll add proper checking later)
    setDirection(desiredDirection);
      }

  // Move in current direction if allowed
  if (canMove && getDirection() != Direction::NONE) {
    switch (getDirection()) {
    case Direction::UP:
      y -= speed * deltaTime;
      break;
    case Direction::DOWN:
      y += speed * deltaTime;
      break;
    case Direction::LEFT:
      x -= speed * deltaTime;
      break;
    case Direction::RIGHT:
      x += speed * deltaTime;
      break;
    case Direction::NONE:
      break;
    }

    setPosition(x, y);
  }
}

