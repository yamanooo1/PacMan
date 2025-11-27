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
  if (pacman) {
    // Calculate where PacMan WOULD move
    auto [x, y] = pacman->getPosition();
    float nextX = x;
    float nextY = y;

    switch (pacman->getDirection()) {
    case Direction::UP:
      nextY = y - pacman->getSpeed() * deltaTime;
      break;
    case Direction::DOWN:
      nextY = y + pacman->getSpeed() * deltaTime;
      break;
    case Direction::LEFT:
      nextX = x - pacman->getSpeed() * deltaTime;
      break;
    case Direction::RIGHT:
      nextX = x + pacman->getSpeed() * deltaTime;
      break;
    case Direction::NONE:
      break;
    }

    // Check if the next position would hit a wall
    // CHANGED: Pass PacMan's bounding box, not just a point
    bool canMove = !isWall(nextX, nextY, pacman->getWidth(), pacman->getHeight());

    // Update PacMan with collision info
    pacman->update(deltaTime, canMove);
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
