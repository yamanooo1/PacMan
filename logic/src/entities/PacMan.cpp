#include "../../include/logic/entities/PacMan.h"
#include <cmath>

PacMan::PacMan(float x, float y)
    : EntityModel(x, y, 0.8f, 0.8f)
    , speed(4.0f)
    , desiredDirection(Direction::LEFT)
    , spawnX(x)
    , spawnY(y) {
  setDirection(Direction::LEFT);
}

void PacMan::setDesiredDirection(Direction dir) {
  desiredDirection = dir;
}

void PacMan::executeTurn() {
  if (desiredDirection != Direction::NONE) {
    setDirection(desiredDirection);
  }
}

void PacMan::die() {
  notify(GameEvent::PACMAN_DIED);
}

void PacMan::respawn() {
  setPosition(spawnX, spawnY);
  setDirection(Direction::LEFT);
  desiredDirection = Direction::NONE;
  notify(GameEvent::DIRECTION_CHANGED);
}

void PacMan::setSpawnPosition(float x, float y) {
  spawnX = x;
  spawnY = y;
}

void PacMan::notifyDirectionChange() {
  notify(GameEvent::DIRECTION_CHANGED);
}

void PacMan::hitWall() {
  notify(GameEvent::WALL_HIT);
}

bool PacMan::isAlignedWithGrid() const {
  auto [x, y] = getPosition();
  constexpr float tolerance = 0.1f;

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