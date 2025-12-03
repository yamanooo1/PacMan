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
#include "logic/Score.h"
#include "logic/Lives.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

World::World(AbstractFactory *f)
    : mapWidth(0)
    , mapHeight(0)
    , factory(f)
    , pacman(nullptr)
    , score(nullptr)
    , lives(nullptr)
    , pacmanSpawnX(0.0f)
    , pacmanSpawnY(0.0f)
    , fearModeActive(false)      // NEW
    , fearModeTimer(0.0f)         // NEW
    , fearModeDuration(7.0f)
{}

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

  // Store spawn position
  pacmanSpawnX = centeredX;
  pacmanSpawnY = centeredY;
  pacman->setSpawnPosition(centeredX, centeredY);

  // Attach Lives as observer if it exists
  if (lives) {
    pacman->attach(lives);
  }

  addEntity(std::move(pacmanModel));
}

// Create Ghost using the factory
void World::createGhost(float x, float y, GhostType type, float waitTime) {
  auto ghost = factory->createGhost(x, y, type, waitTime);
  Ghost* ghostPtr = ghost.get();

  // Center ghost in grid cell
  float centeredX = x + (1.0f - ghost->getWidth()) / 2.0f;
  float centeredY = y + (1.0f - ghost->getHeight()) / 2.0f;
  ghost->setPosition(centeredX, centeredY);

  // Debug output
  float centerX = centeredX + ghost->getWidth() / 2.0f;
  float centerY = centeredY + ghost->getHeight() / 2.0f;
  std::cout << "[SPAWN] Ghost type " << static_cast<int>(type)
            << " at grid (" << x << "," << y << ")"
            << " position (" << centeredX << "," << centeredY << ")"
            << " center (" << centerX << "," << centerY << ")" << std::endl;

  // ✅ CRITICAL FIX: Attach Score as observer
  // This ensures Score receives GHOST_EATEN events and awards +200 points
  if (score) {
    ghost->attach(score);
  }

  // Store spawn position for respawning
  ghostSpawnPositions.push_back({centeredX, centeredY});

  // Track ghost for updates
  ghosts.push_back(ghostPtr);

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

  // Center coin in the grid cell (just like PacMan)
  float centeredX = x + (1.0f - coin->getWidth()) / 2.0f;
  float centeredY = y + (1.0f - coin->getHeight()) / 2.0f;
  coin->setPosition(centeredX, centeredY);

  // Attach Score as observer if it exists
  if (score) {
    coin->attach(score);
  }

  addEntity(std::move(coin));
}

// Create Fruit using the factory
void World::createFruit(float x, float y) {
  auto fruit = factory->createFruit(x, y);

  // Center fruit in the grid cell
  float centeredX = x + (1.0f - fruit->getWidth()) / 2.0f;
  float centeredY = y + (1.0f - fruit->getHeight()) / 2.0f;
  fruit->setPosition(centeredX, centeredY);

  // Attach Score as observer if it exists
  if (score) {
    fruit->attach(score);
  }

  // NEW: Store pointer to fruit so we can detect when it's collected
  Fruit* fruitPtr = fruit.get();

  addEntity(std::move(fruit));
}

bool World::loadFromFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open map file: " << filename << std::endl;
    return false;
  }

  // Clear existing entities and spawn positions
  entities.clear();
  pacman = nullptr;
  ghosts.clear();
  ghostSpawnPositions.clear();

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
      case 'r':  // Red ghost: Chaser, leaves immediately
        createGhost(x, y, GhostType::CHASER, 0.0f);
        break;
      case 'b':  // Blue ghost: Ambusher, leaves immediately
        createGhost(x, y, GhostType::AMBUSHER, 0.0f);
        break;
      case 'o':  // Orange ghost: Ambusher, leaves after 5s
        createGhost(x, y, GhostType::AMBUSHER, 5.0f);
        break;
      case 'p':  // Pink ghost: Random, leaves after 10s
        createGhost(x, y, GhostType::RANDOM, 10.0f);
        break;
      case 'f':  // lowercase 'f'
        createFruit(x, y);
        break;
      case 'w':  // Exit gate
        exitPositions.push_back({col, row});
        std::cout << "[MAP] Exit gate at (" << col << "," << row << ")" << std::endl;
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

void World::respawnPacManAndGhosts() {
  if (!pacman) return;

  std::cout << "[WORLD] Respawning PacMan and Ghosts..." << std::endl;

  // Respawn PacMan
  pacman->respawn();

  // Respawn all ghosts to their spawn positions
  int ghostIndex = 0;
  for (auto& entity : entities) {
    // Check if this is a ghost (size ~0.05)
    if (entity->getWidth() < 0.1f && entity->getWidth() > 0.04f) {
      if (ghostIndex < ghostSpawnPositions.size()) {
        auto [spawnX, spawnY] = ghostSpawnPositions[ghostIndex];
        entity->setPosition(spawnX, spawnY);
        entity->setDirection(Direction::NONE);
        ghostIndex++;
      }
    }
  }
}

// World.cpp - IMPROVED update() method
// Shows where to call pacman->notifyDirectionChange()

void World::update(float deltaTime) {
  if (!pacman) return;

  if (lives && lives->isGameOver()) {
    std::cout << "[WORLD] Game Over! Press ESC to quit." << std::endl;
    return;
  }

  auto [x, y] = pacman->getPosition();
  Direction currentDir = pacman->getDirection();
  Direction desiredDir = pacman->getDesiredDirection();

  float centerX = x + pacman->getWidth() / 2.0f;
  float centerY = y + pacman->getHeight() / 2.0f;

  int currentGridX = static_cast<int>(std::floor(centerX));
  int currentGridY = static_cast<int>(std::floor(centerY));
  float gridCenterX = currentGridX + 0.5f;
  float gridCenterY = currentGridY + 0.5f;

  const float centerTolerance = 0.05f;
  bool atCenterX = std::abs(centerX - gridCenterX) < centerTolerance;
  bool atCenterY = std::abs(centerY - gridCenterY) < centerTolerance;
  bool atCenter = atCenterX && atCenterY;

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
    if (!isPerpendicular || atCenter) {
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
        if (isPerpendicular && atCenter) {
          float exactCenterX = currentGridX + 0.5f;
          float exactCenterY = currentGridY + 0.5f;
          float exactX = exactCenterX - pacman->getWidth() / 2.0f;
          float exactY = exactCenterY - pacman->getHeight() / 2.0f;
          pacman->setPosition(exactX, exactY);
        }

        // ✅ CRITICAL: Notify observers AFTER successful turn
        pacman->executeTurn();
        pacman->notifyDirectionChange();  // ← ADD THIS LINE!

        currentDir = desiredDir;
      }
    }
  }

  // ... rest of update logic (movement, ghosts, collisions) ...

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
    pacman->update(deltaTime, true);
  }

  updateGhosts(deltaTime);
  updateFearMode(deltaTime);
  checkCollisions();
  removeDeadEntities();
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
void World::checkCollisions() {
  if (!pacman) return;

  auto [px, py] = pacman->getPosition();
  float pw = pacman->getWidth();
  float ph = pacman->getHeight();

  bool pacmanDied = false;

  // Check collision with all entities
  for (auto& entity : entities) {
    if (entity->isDead()) continue;
    if (entity.get() == pacman) continue;

    auto [ex, ey] = entity->getPosition();
    float ew = entity->getWidth();
    float eh = entity->getHeight();

    // AABB collision detection
    bool collision = (px < ex + ew &&
                      px + pw > ex &&
                      py < ey + eh &&
                      py + ph > ey);

    if (collision) {
      // Check if it's a ghost (size ~0.8)
      if (ew > 0.7f && ew < 0.9f && eh > 0.7f && eh < 0.9f) {
        // Ghost collision - check in ghosts vector
        Ghost* collidedGhost = nullptr;
        int ghostIndex = -1;

        for (size_t i = 0; i < ghosts.size(); ++i) {
          auto [gx, gy] = ghosts[i]->getPosition();
          if (std::abs(gx - ex) < 0.01f && std::abs(gy - ey) < 0.01f) {
            collidedGhost = ghosts[i];
            ghostIndex = i;
            break;
          }
        }

        if (collidedGhost && collidedGhost->isInFearMode()) {
          // PacMan eats ghost in fear mode!
          std::cout << "[COLLISION] PacMan ate a ghost!" << std::endl;
          collidedGhost->onEaten();

          // Respawn ghost in center
          if (ghostIndex >= 0 && ghostIndex < ghostSpawnPositions.size()) {
            auto [spawnX, spawnY] = ghostSpawnPositions[ghostIndex];
            collidedGhost->setPosition(spawnX, spawnY);
            collidedGhost->setDirection(Direction::UP);
            collidedGhost->exitFearMode();
          }
        } else {
          // Normal ghost - PacMan dies!
          pacmanDied = true;
          std::cout << "[COLLISION] Ghost hit PacMan!" << std::endl;
        }
      }
      // Check if it's a fruit (size ~0.03)
      else if (ew > 0.02f && ew < 0.04f && eh > 0.02f && eh < 0.04f) {
        // Fruit collision!
        std::cout << "[COLLISION] PacMan ate a fruit!" << std::endl;
        entity->onCollisionWithPacMan();
        activateFearMode();
      }
      else {
        // Other entity (coin)
        entity->onCollisionWithPacMan();
      }
    }
  }

  // Handle death after collision loop
  if (pacmanDied && lives) {
    pacman->die();

    if (!lives->isGameOver()) {
      respawnPacManAndGhosts();
    }
  }
}

void World::removeDeadEntities() {
  // First remove views
  factory->removeDeadViews();

  // Then remove the models themselves
  entities.erase(
    std::remove_if(entities.begin(), entities.end(),
      [](const std::unique_ptr<EntityModel>& entity) {
        return entity->isDead();
      }),
    entities.end()
  );
}

void World::activateFearMode() {
  fearModeActive = true;
  fearModeTimer = fearModeDuration;

  std::cout << "[WORLD] Fear mode activated for " << fearModeDuration << " seconds!" << std::endl;

  // Put all ghosts in fear mode
  for (Ghost* ghost : ghosts) {
    if (ghost) {
      ghost->enterFearMode();
    }
  }
}

void World::updateFearMode(float deltaTime) {
  if (!fearModeActive) return;

  fearModeTimer -= deltaTime;

  if (fearModeTimer <= 0.0f) {
    fearModeActive = false;
    std::cout << "[WORLD] Fear mode ended!" << std::endl;

    // Return all ghosts to chasing mode
    for (Ghost* ghost : ghosts) {
      if (ghost) {
        ghost->exitFearMode();
      }
    }
  }
}

// Add these methods at the end of the file, before the closing brace

bool World::isExitPosition(int gridX, int gridY) const {
  for (const auto& [ex, ey] : exitPositions) {
    if (ex == gridX && ey == gridY) {
      return true;
    }
  }
  return false;
}

std::vector<std::pair<int, int>> World::getExitPositions() const {
  return exitPositions;
}

void World::updateGhosts(float deltaTime) {
  for (Ghost* ghost : ghosts) {
    if (ghost) {
      ghost->update(deltaTime, this, pacman);
    }
  }
}