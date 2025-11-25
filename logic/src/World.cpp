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

World::World(AbstractFactory *f) : mapWidth(0), mapHeight(0) , factory(f){} //start with empty world

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
  // Factory creates PacMan (and its view automatically)
  auto pacman = factory->createPacMan(x, y);

  // World takes ownership of the model
  addEntity(std::move(pacman));
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
      case 'W':
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

