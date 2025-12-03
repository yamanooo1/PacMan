//
// World.cpp - WITH DEATH ANIMATION
// Implements proper death sequence: die → animate → reset
//

#include "logic/World.h"
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
    , fearModeActive(false)
    , fearModeTimer(0.0f)
    , fearModeDuration(7.0f)
    , deathAnimationActive(false)      // ✅ NEW
    , deathAnimationTimer(0.0f)        // ✅ NEW
    , deathAnimationDuration(2.0f)     // ✅ NEW: 2 second death animation
{}

void World::setMapDimensions(int width, int height) {
  mapWidth = width;
  mapHeight = height;
}

void World::addEntity(std::unique_ptr<EntityModel> entity) {
  entities.push_back(std::move(entity));
}

void World::createPacMan(float x, float y) {
  auto pacmanModel = factory->createPacMan(x, y);
  pacman = pacmanModel.get();

  float centeredX = x + (1.0f - pacman->getWidth()) / 2.0f;
  float centeredY = y + (1.0f - pacman->getHeight()) / 2.0f;
  pacman->setPosition(centeredX, centeredY);

  pacmanSpawnX = centeredX;
  pacmanSpawnY = centeredY;
  pacman->setSpawnPosition(centeredX, centeredY);

  if (lives) {
    pacman->attach(lives);
  }

  addEntity(std::move(pacmanModel));
}

void World::createGhost(float x, float y, GhostType type, float waitTime) {
  auto ghost = factory->createGhost(x, y, type, waitTime);
  Ghost* ghostPtr = ghost.get();

  float centeredX = x + (1.0f - ghost->getWidth()) / 2.0f;
  float centeredY = y + (1.0f - ghost->getHeight()) / 2.0f;
  ghost->setPosition(centeredX, centeredY);

  float centerX = centeredX + ghost->getWidth() / 2.0f;
  float centerY = centeredY + ghost->getHeight() / 2.0f;
  std::cout << "[SPAWN] Ghost type " << static_cast<int>(type)
            << " at grid (" << x << "," << y << ")"
            << " position (" << centeredX << "," << centeredY << ")"
            << " center (" << centerX << "," << centerY << ")" << std::endl;

  if (score) {
    ghost->attach(score);
  }

  ghostSpawnPositions.push_back({centeredX, centeredY});
  ghosts.push_back(ghostPtr);
  addEntity(std::move(ghost));
}

void World::createWall(float x, float y, float w, float h) {
  auto wall = factory->createWall(x, y, w, h);
  addEntity(std::move(wall));
}

void World::createCoin(float x, float y) {
  auto coin = factory->createCoin(x, y);

  float centeredX = x + (1.0f - coin->getWidth()) / 2.0f;
  float centeredY = y + (1.0f - coin->getHeight()) / 2.0f;
  coin->setPosition(centeredX, centeredY);

  if (score) {
    coin->attach(score);
  }

  addEntity(std::move(coin));
}

void World::createFruit(float x, float y) {
  auto fruit = factory->createFruit(x, y);

  float centeredX = x + (1.0f - fruit->getWidth()) / 2.0f;
  float centeredY = y + (1.0f - fruit->getHeight()) / 2.0f;
  fruit->setPosition(centeredX, centeredY);

  if (score) {
    fruit->attach(score);
  }

  Fruit* fruitPtr = fruit.get();
  addEntity(std::move(fruit));
}

bool World::loadFromFile(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open map file: " << filename << std::endl;
    return false;
  }

  entities.clear();
  pacman = nullptr;
  ghosts.clear();
  ghostSpawnPositions.clear();

  file >> mapWidth >> mapHeight;
  file.ignore();

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
      case 'r':
        createGhost(x, y, GhostType::CHASER, 0.0f);
        break;
      case 'b':
        createGhost(x, y, GhostType::AMBUSHER, 0.0f);
        break;
      case 'o':
        createGhost(x, y, GhostType::AMBUSHER, 5.0f);
        break;
      case 'p':
        createGhost(x, y, GhostType::RANDOM, 10.0f);
        break;
      case 'f':
        createFruit(x, y);
        break;
      case 'w':
        exitPositions.push_back({col, row});
        std::cout << "[MAP] Exit gate at (" << col << "," << row << ")" << std::endl;
        break;
      case ' ':
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

  pacman->respawn();

  // ✅ Respawn all ghosts and reset their spawn flags
  for (size_t i = 0; i < ghosts.size(); ++i) {
    if (i < ghostSpawnPositions.size()) {
      auto [spawnX, spawnY] = ghostSpawnPositions[i];
      ghosts[i]->setPosition(spawnX, spawnY);
      ghosts[i]->setDirection(Direction::UP);
      ghosts[i]->resetSpawnFlag();  // Reset to EXITING state
    }
  }
}

void World::update(float deltaTime) {
  if (!pacman) return;

  // ✅ NEW: Handle death animation - freeze game during animation
  if (deathAnimationActive) {
    updateDeathAnimation(deltaTime);
    return;  // Don't update anything else during death animation
  }

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

        pacman->executeTurn();
        pacman->notifyDirectionChange();
        currentDir = desiredDir;
      }
    }
  }

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
  checkLevelComplete();
}

bool World::isWall(float x, float y, float width, float height) const {
  for (const auto& entity : entities) {
    auto [ex, ey] = entity->getPosition();
    float ew = entity->getWidth();
    float eh = entity->getHeight();

    if (ew < 0.9f || eh < 0.9f) {
      continue;
    }

    if (x < ex + ew &&
        x + width > ex &&
        y < ey + eh &&
        y + height > ey) {
      return true;
    }
  }
  return false;
}

bool World::hasWallInGridCell(int gridX, int gridY) const {
  for (const auto& entity : entities) {
    auto [ex, ey] = entity->getPosition();
    float ew = entity->getWidth();
    float eh = entity->getHeight();

    if (ew < 0.9f || eh < 0.9f) {
      continue;
    }

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

  for (auto& entity : entities) {
    if (entity->isDead()) continue;
    if (entity.get() == pacman) continue;

    auto [ex, ey] = entity->getPosition();
    float ew = entity->getWidth();
    float eh = entity->getHeight();

    bool collision = (px < ex + ew &&
                      px + pw > ex &&
                      py < ey + eh &&
                      py + ph > ey);

    if (collision) {
      if (ew > 0.7f && ew < 0.9f && eh > 0.7f && eh < 0.9f) {
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
          std::cout << "[COLLISION] PacMan ate a ghost!" << std::endl;
          collidedGhost->onEaten();

          if (ghostIndex >= 0 && ghostIndex < ghostSpawnPositions.size()) {
            auto [spawnX, spawnY] = ghostSpawnPositions[ghostIndex];
            collidedGhost->setPosition(spawnX, spawnY);
            collidedGhost->setDirection(Direction::UP);
            collidedGhost->exitFearMode();
            collidedGhost->resetSpawnFlag();
          }
        } else {
          pacmanDied = true;
          std::cout << "[COLLISION] Ghost hit PacMan!" << std::endl;
        }
      }
      else if (ew > 0.02f && ew < 0.04f && eh > 0.02f && eh < 0.04f) {
        std::cout << "[COLLISION] PacMan ate a fruit!" << std::endl;
        entity->onCollisionWithPacMan();
        activateFearMode();
      }
      else {
        entity->onCollisionWithPacMan();
      }
    }
  }

  // ✅ NEW: Start death animation instead of immediate respawn
  if (pacmanDied && lives) {
    pacman->die();  // Notify observers (Lives, Views)

    if (!lives->isGameOver()) {
      startDeathAnimation();  // Start animation, respawn happens after
    }
  }
}

void World::removeDeadEntities() {
  factory->removeDeadViews();

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

    for (Ghost* ghost : ghosts) {
      if (ghost) {
        ghost->exitFearMode();
      }
    }
  }
}

// ✅ NEW: Start death animation
void World::startDeathAnimation() {
  deathAnimationActive = true;
  deathAnimationTimer = deathAnimationDuration;

  std::cout << "[WORLD] Death animation started (" << deathAnimationDuration << " seconds)" << std::endl;
}

// ✅ NEW: Update death animation timer
void World::updateDeathAnimation(float deltaTime) {
  if (!deathAnimationActive) return;

  deathAnimationTimer -= deltaTime;

  if (deathAnimationTimer <= 0.0f) {
    deathAnimationActive = false;

    std::cout << "[WORLD] Death animation complete - respawning!" << std::endl;

    // Now respawn everything
    respawnPacManAndGhosts();
  }
}

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

void World::checkLevelComplete() {
  int remainingCollectables = 0;

  for (const auto& entity : entities) {
    if (entity->isDead()) continue;

    float w = entity->getWidth();
    float h = entity->getHeight();

    bool isCoin = (w > 0.015f && w < 0.025f && h > 0.015f && h < 0.025f);
    bool isFruit = (w > 0.025f && w < 0.04f && h > 0.025f && h < 0.04f);

    if (isCoin || isFruit) {
      remainingCollectables++;
    }
  }

  if (remainingCollectables == 0) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "       🎉 LEVEL CLEARED! 🎉" << std::endl;
    std::cout << "========================================\n" << std::endl;

    if (score) {
      score->update(GameEvent::LEVEL_CLEARED);
    }

    std::cout << "[WORLD] TODO: Load next level with increased difficulty" << std::endl;
    std::cout << "[WORLD] Press ESC to quit for now" << std::endl;
  }
}