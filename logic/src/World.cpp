//
// Created by yamanooo on 11/22/25.
//

#include "logic/World.h"
// Include complete types so unique_ptr can work with them
#include "logic/PacMan.h"
#include "logic/Ghost.h"
#include "logic/Wall.h"
#include "logic/Coin.h"
#include "logic/Fruit.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

World::World(AbstractFactory *f) : mapWidth(0), mapHeight(0) , factory(f), pacman(nullptr){} //start with empty world

// Set the dimensions of the map (called when loading from file)
void World::setMapDimensions(int width, int height) {
  mapWidth = width;
  mapHeight = height;
}

// Add an entity to the world
// Takes ownership via unique_ptr
void World::addEntity(std::unique_ptr<EntityModel> entity) {
  entities.push_back(std::move(entity));
}

// Create PacMan using the factory
void World::createPacMan(float x, float y) {
  auto pacmanModel = factory->createPacMan(x, y);
  pacman = pacmanModel.get();

  // Center PacMan in the grid cell (grid cell size is 1.0 x 1.0)
  float centeredX = x + (1.0f - pacman->getWidth()) / 2.0f;
  float centeredY = y + (1.0f - pacman->getHeight()) / 2.0f;
  pacman->setPosition(centeredX, centeredY);

  addEntity(std::move(pacmanModel));
}

// Create Ghost using the factory
void World::createGhost(float x, float y) {
  auto ghost = factory->createGhost(x, y);
  addEntity(std::move(ghost));
}

// Create Wall using the factory
void World::createWall(float x, float y, float w, float h) {
  auto wall = factory->createWall(x, y, w, h);
  addEntity(std::move(wall));
}

// Create Coin using the factory
void World::createCoin(float x, float y) {
  auto coin = factory->createCoin(x, y);
  addEntity(std::move(coin));
}

// Create Fruit using the factory
void World::createFruit(float x, float y) {
  auto fruit = factory->createFruit(x, y);
  addEntity(std::move(fruit));
}
bool World::loadFromFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open map file: " << filename << std::endl;
    return false;
  }

  // Clear existing entities
  entities.clear();
  pacman = nullptr;

  // Read map dimensions
  file >> mapWidth >> mapHeight;
  file.ignore();  // Skip newline after dimensions

  // Read the grid
  std::string line;
  int row = 0;
  while (std::getline(file, line) && row < mapHeight) {
    for (int col = 0; col < line.length() && col < mapWidth; ++col) {
      char c = line[col];
      float x = static_cast<float>(col);
      float y = static_cast<float>(row);

      switch (c) {
      case 'x':
        createWall(x, y, 1.0f, 1.0f);
        break;
      case '.':
        createCoin(x, y);
        break;
      case 'P':
        createPacMan(x, y);
        break;
      case 'G':
        createGhost(x, y);
        break;
      case 'F':
        createFruit(x, y);
        break;
      case ' ':
        // Empty space
        break;
      }
    }
    row++;
  }

  file.close();
  return true;
}

void World::update(float deltaTime) {
  if (!pacman) return;

  auto [x, y] = pacman->getPosition();
  Direction currentDir = pacman->getDirection();
  Direction desiredDir = pacman->getDesiredDirection();

  // Calculate PacMan's center
  float centerX = x + pacman->getWidth() / 2.0f;
  float centerY = y + pacman->getHeight() / 2.0f;

  // Determine current grid cell
  int currentGridX = static_cast<int>(std::floor(centerX));
  int currentGridY = static_cast<int>(std::floor(centerY));
  float gridCenterX = currentGridX + 0.5f;
  float gridCenterY = currentGridY + 0.5f;

  // Check if at center
  const float centerTolerance = 0.05f;
  bool atCenterX = std::abs(centerX - gridCenterX) < centerTolerance;
  bool atCenterY = std::abs(centerY - gridCenterY) < centerTolerance;
  bool atCenter = atCenterX && atCenterY;

  // Check if desired direction is a perpendicular turn (90 degrees)
  bool isPerpendicular = false;
  if (desiredDir != Direction::NONE && desiredDir != currentDir) {
    if ((currentDir == Direction::UP || currentDir == Direction::DOWN) &&
        (desiredDir == Direction::LEFT || desiredDir == Direction::RIGHT)) {
      isPerpendicular = true;
    } else if ((currentDir == Direction::LEFT || currentDir == Direction::RIGHT) &&
               (desiredDir == Direction::UP || desiredDir == Direction::DOWN)) {
      isPerpendicular = true;
    }
  }

  // Try to turn
  if (desiredDir != Direction::NONE && desiredDir != currentDir) {
    // For perpendicular turns, must be at center. For reversals, can turn immediately
    if (!isPerpendicular || atCenter) {
      // Check if desired direction leads to a wall
      int desiredGridX = currentGridX;
      int desiredGridY = currentGridY;

      switch (desiredDir) {
        case Direction::UP:    desiredGridY--; break;
        case Direction::DOWN:  desiredGridY++; break;
        case Direction::LEFT:  desiredGridX--; break;
        case Direction::RIGHT: desiredGridX++; break;
        case Direction::NONE: break;
      }

      bool wallInDesiredDirection = hasWallInGridCell(desiredGridX, desiredGridY);

      if (!wallInDesiredDirection) {
        // Only snap if it's a perpendicular turn at center
        if (isPerpendicular && atCenter) {
          float exactCenterX = currentGridX + 0.5f;
          float exactCenterY = currentGridY + 0.5f;
          float exactX = exactCenterX - pacman->getWidth() / 2.0f;
          float exactY = exactCenterY - pacman->getHeight() / 2.0f;
          pacman->setPosition(exactX, exactY);
        }

        // Execute the turn
        pacman->executeTurn();
        currentDir = desiredDir;
      }
    }
  }

  // Now handle movement in current direction
  int nextGridX = currentGridX;
  int nextGridY = currentGridY;

  switch (currentDir) {
    case Direction::UP:    nextGridY--; break;
    case Direction::DOWN:  nextGridY++; break;
    case Direction::LEFT:  nextGridX--; break;
    case Direction::RIGHT: nextGridX++; break;
    case Direction::NONE: break;
  }

  bool wallAhead = hasWallInGridCell(nextGridX, nextGridY);

  if (wallAhead) {
    // Wall ahead - move toward center then stop
    float targetCenterX = currentGridX + 0.5f;
    float targetCenterY = currentGridY + 0.5f;

    bool atTargetCenter = (std::abs(centerX - targetCenterX) < 0.01f &&
                           std::abs(centerY - targetCenterY) < 0.01f);

    if (!atTargetCenter) {
      float speed = pacman->getSpeed();
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

      float newX = newCenterX - pacman->getWidth() / 2.0f;
      float newY = newCenterY - pacman->getHeight() / 2.0f;
      pacman->setPosition(newX, newY);
    }

    pacman->update(deltaTime, false);
  } else {
    // No wall, move normally
    pacman->update(deltaTime, true);
  }
}

bool World::isWall(float x, float y, float width, float height) const {
  // Check all entities to see if any wall overlaps with the given rectangle
  for (const auto& entity : entities) {
    auto [ex, ey] = entity->getPosition();
    float ew = entity->getWidth();
    float eh = entity->getHeight();

    // Skip if not a wall (walls are approximately 1x1)
    if (ew < 0.9f || eh < 0.9f) {
      continue;
    }

    // AABB (Axis-Aligned Bounding Box) collision detection
    // Check if rectangles overlap
    if (x < ex + ew &&      // Left edge of PacMan is left of right edge of wall
        x + width > ex &&   // Right edge of PacMan is right of left edge of wall
        y < ey + eh &&      // Top edge of PacMan is above bottom edge of wall
        y + height > ey) {  // Bottom edge of PacMan is below top edge of wall
      return true;  // Collision detected!
        }
  }
  return false;
}

bool World::hasWallInGridCell(int gridX, int gridY) const {
  // Check if there's a wall entity at this grid position
  for (const auto& entity : entities) {
    auto [ex, ey] = entity->getPosition();
    float ew = entity->getWidth();
    float eh = entity->getHeight();

    // Only check walls (walls are 1x1)
    if (ew < 0.9f || eh < 0.9f) {
      continue;
    }

    // Check if this wall occupies the grid cell
    int wallGridX = static_cast<int>(std::floor(ex));
    int wallGridY = static_cast<int>(std::floor(ey));

    if (wallGridX == gridX && wallGridY == gridY) {
      return true;
    }
  }
  return false;
}
