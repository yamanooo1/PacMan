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

World::World(AbstractFactory *f, int level)
    : mapWidth(0)
    , mapHeight(0)
    , currentLevel(level)
    , ghostSpeedMultiplier(1.0f + (level - 1) * 0.09f)  // ✅ 5% faster per level
    , fearDurationMultiplier(1.0f - (level - 1) * 0.09f)  // ✅ 5% shorter per level
    , factory(f)
    , pacman(nullptr)
    , score(nullptr)
    , lives(nullptr)
    , pacmanSpawnX(0.0f)
    , pacmanSpawnY(0.0f)
    , fearModeActive(false)
    , fearModeTimer(0.0f)
    , fearModeDuration(7.0f * fearDurationMultiplier)  // ✅ Apply multiplier
    , deathAnimationActive(false)
    , deathAnimationTimer(0.0f)
    , deathAnimationDuration(2.0f)
    , readyStateActive(false)
    , readyStateTimer(0.0f)
    , readyStateDuration(4.7f)
    , levelCleared(false)
    , levelClearedDisplayActive(false)
    , levelClearedDisplayTimer(0.0f)
    , levelClearedDisplayDuration(3.0f)
{
  std::cout << "[WORLD] Level " << currentLevel
            << " - Ghost speed: x" << ghostSpeedMultiplier
            << " Fear duration: x" << fearDurationMultiplier << std::endl;
}

void World::startReadyState() {
  readyStateActive = true;
  readyStateTimer = readyStateDuration;
  std::cout << "[WORLD] READY state started (" << readyStateDuration << " seconds)" << std::endl;
}

void World::updateReadyState(float deltaTime) {
  if (!readyStateActive) return;

  readyStateTimer -= deltaTime;

  if (readyStateTimer <= 0.0f) {
    readyStateActive = false;
    std::cout << "[WORLD] READY state complete - game starting!" << std::endl;
  }
}

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

  // ✅ CORRECTED: Use shared_from_this() to attach
  if (lives) {
    pacman->attach(lives->shared_from_this());
  }

  addEntity(std::move(pacmanModel));
}

void World::createGhost(float x, float y, GhostType type, GhostColor color, float waitTime) {
  auto ghost = factory->createGhost(x, y, type, color, waitTime, ghostSpeedMultiplier);
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

  // ✅ CORRECTED: Use shared_from_this() to attach
  if (score) {
    ghost->attach(score->shared_from_this());
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

  // ✅ CORRECTED: Use shared_from_this() to attach
  if (score) {
    coin->attach(score->shared_from_this());
  }

  addEntity(std::move(coin));
}

void World::createFruit(float x, float y) {
  auto fruit = factory->createFruit(x, y);

  float centeredX = x + (1.0f - fruit->getWidth()) / 2.0f;
  float centeredY = y + (1.0f - fruit->getHeight()) / 2.0f;
  fruit->setPosition(centeredX, centeredY);

  // ✅ CORRECTED: Use shared_from_this() to attach
  if (score) {
    fruit->attach(score->shared_from_this());
  }

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
        createGhost(x, y, GhostType::CHASER, GhostColor::RED, 0.0f);
        break;
      case 'b':
        createGhost(x, y, GhostType::AMBUSHER, GhostColor::CYAN, 0.6f);
        break;
      case 'o':
        createGhost(x, y, GhostType::CHASER, GhostColor::ORANGE, 5.0f);
        break;
      case 'p':
        createGhost(x, y, GhostType::RANDOM, GhostColor::PINK, 10.0f);
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

  // ✅ Exit fear mode for all ghosts on respawn
  for (Ghost* ghost : ghosts) {
    if (ghost) {
      ghost->exitFearMode();
    }
  }

  // Reset ghost positions and states
  for (size_t i = 0; i < ghosts.size(); ++i) {
    if (i < ghostSpawnPositions.size()) {
      auto [spawnX, spawnY] = ghostSpawnPositions[i];
      ghosts[i]->setPosition(spawnX, spawnY);
      ghosts[i]->setDirection(Direction::UP);
      ghosts[i]->resetSpawnFlag();
    }
  }
}


void World::update(float deltaTime) {
  if (!pacman) return;

  // ✅ Handle ready state FIRST - freeze game during "READY!"
  if (readyStateActive) {
    updateReadyState(deltaTime);
    return;  // Don't update anything else during ready state
  }

  // ✅ Handle death animation - freeze game during animation
  if (deathAnimationActive) {
    updateDeathAnimation(deltaTime);
    return;  // Don't update anything else during death animation
  }

  // ✅ Handle level cleared display - freeze game and count down timer
  if (levelClearedDisplayActive) {
    levelClearedDisplayTimer -= deltaTime;
    if (levelClearedDisplayTimer <= 0.0f) {
      levelClearedDisplayActive = false;
      std::cout << "[WORLD] ✅ 3-second timer EXPIRED! Display inactive now." << std::endl;
    }
    return;  // Freeze all game logic during display
  }

  if (lives && lives->isGameOver()) {
    std::cout << "[WORLD] Game Over! Press ESC to quit." << std::endl;
    return;
  }

  auto [x, y] = pacman->getPosition();
  Direction currentDir = pacman->getDirection();
  Direction desiredDir = pacman->getDesiredDirection();

  static int pacmanFrameCount = 0;
  pacmanFrameCount++;
  if (pacmanFrameCount % 60 == 0) {
    std::cout << "[DEBUG PacMan] Frame " << pacmanFrameCount
              << " | Pos(" << x << "," << y << ")"
              << " | CurrentDir=" << static_cast<int>(currentDir)
              << " | DesiredDir=" << static_cast<int>(desiredDir)
              << " | Speed=" << pacman->getSpeed()
              << std::endl;
  }

  float centerX = x + pacman->getWidth() / 2.0f;
  float centerY = y + pacman->getHeight() / 2.0f;

  int currentGridX = static_cast<int>(std::floor(centerX));
  int currentGridY = static_cast<int>(std::floor(centerY));
  float gridCenterX = currentGridX + 0.5f;
  float gridCenterY = currentGridY + 0.5f;

  // Store previous position for crossing detection
  static float prevCenterX = centerX;
  static float prevCenterY = centerY;

  // Check if we're at or crossed the center
  float speed = pacman->getSpeed();
  float maxMovePerFrame = speed * deltaTime;
  const float centerTolerance = std::max(0.15f, maxMovePerFrame * 1.5f);

  bool atCenterX = std::abs(centerX - gridCenterX) < centerTolerance;
  bool atCenterY = std::abs(centerY - gridCenterY) < centerTolerance;

  // Check if we crossed the center line between frames
  bool crossedCenterX = false;
  bool crossedCenterY = false;

  if (currentDir == Direction::LEFT || currentDir == Direction::RIGHT) {
    // Moving horizontally - check if we crossed the vertical center line
    if ((prevCenterX < gridCenterX && centerX >= gridCenterX) ||
        (prevCenterX > gridCenterX && centerX <= gridCenterX)) {
      crossedCenterX = true;
      atCenterX = true;  // Consider it centered if we crossed
    }
  }

  if (currentDir == Direction::UP || currentDir == Direction::DOWN) {
    // Moving vertically - check if we crossed the horizontal center line
    if ((prevCenterY < gridCenterY && centerY >= gridCenterY) ||
        (prevCenterY > gridCenterY && centerY <= gridCenterY)) {
      crossedCenterY = true;
      atCenterY = true;  // Consider it centered if we crossed
    }
  }

  bool atCenter = atCenterX && atCenterY;

  // Update previous position for next frame
  prevCenterX = centerX;
  prevCenterY = centerY;

  // Check desired direction EVERY frame (not just when centered)
  if (desiredDir != Direction::NONE && desiredDir != currentDir) {
    int testX = currentGridX;
    int testY = currentGridY;

    switch (desiredDir) {
      case Direction::UP:    testY--; break;
      case Direction::DOWN:  testY++; break;
      case Direction::LEFT:  testX--; break;
      case Direction::RIGHT: testX++; break;
      case Direction::NONE:  break;
    }

    // Check if the desired path is clear
    bool pathIsClear = !hasWallInGridCell(testX, testY);

    if (pathIsClear) {
      // Determine if this is a perpendicular turn
      bool isPerpendicular = false;
      if ((currentDir == Direction::UP || currentDir == Direction::DOWN) &&
          (desiredDir == Direction::LEFT || desiredDir == Direction::RIGHT)) {
        isPerpendicular = true;
      }
      else if ((currentDir == Direction::LEFT || currentDir == Direction::RIGHT) &&
               (desiredDir == Direction::UP || desiredDir == Direction::DOWN)) {
        isPerpendicular = true;
      }

      // Decide if we can turn now
      bool canTurn = false;

      if (currentDir == Direction::NONE) {
        // Starting from stopped - can always turn
        canTurn = true;
      }
      else if (!isPerpendicular) {
        // Opposite direction (180° turn) - turn immediately
        canTurn = true;
      }
      else if (atCenter) {
        // Perpendicular turn - only when centered to avoid corner cutting
        canTurn = true;
      }

      if (canTurn) {
        // Snap to grid center when turning to ensure perfect centering
        float newX = gridCenterX - pacman->getWidth() / 2.0f;
        float newY = gridCenterY - pacman->getHeight() / 2.0f;
        pacman->setPosition(newX, newY);

        pacman->setDirection(desiredDir);
        currentDir = desiredDir;
      }
    }
  }

  // Check for walls ahead in current direction
  int testX = currentGridX;
  int testY = currentGridY;

  switch (currentDir) {
    case Direction::UP:    testY--; break;
    case Direction::DOWN:  testY++; break;
    case Direction::LEFT:  testX--; break;
    case Direction::RIGHT: testX++; break;
    case Direction::NONE:  break;
  }

  if (hasWallInGridCell(testX, testY)) {
    // Check if Pac-Man's edge would collide with the wall
    float halfWidth = pacman->getWidth() / 2.0f;
    float halfHeight = pacman->getHeight() / 2.0f;
    bool shouldStop = false;

    switch (currentDir) {
      case Direction::LEFT:
        // Wall is to the left, stop if left edge would cross the boundary
        if (centerX <= (testX + 1.0f) + halfWidth) {
          shouldStop = true;
        }
        break;
      case Direction::RIGHT:
        // Wall is to the right, stop if right edge would cross the boundary
        if (centerX >= testX - halfWidth) {
          shouldStop = true;
        }
        break;
      case Direction::UP:
        // Wall is above, stop if top edge would cross the boundary
        if (centerY <= (testY + 1.0f) + halfHeight) {
          shouldStop = true;
        }
        break;
      case Direction::DOWN:
        // Wall is below, stop if bottom edge would cross the boundary
        if (centerY >= testY - halfHeight) {
          shouldStop = true;
        }
        break;
      case Direction::NONE:
        break;
    }

    if (shouldStop) {
      pacman->setDirection(Direction::NONE);
      pacman->hitWall();  // ← THIS LINE MUST BE HERE!
    }
  }

  // Normal movement - no special sliding logic
  pacman->update(deltaTime, true);

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
            collidedGhost->respawnAfterEaten();

            std::cout << "[GHOST] Ghost respawned after being eaten - will exit immediately" << std::endl;
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

  if (pacmanDied && lives) {
    pacman->die();

    // Deactivate fear mode immediately when PacMan dies
    if (fearModeActive) {
      fearModeActive = false;
      fearModeTimer = 0.0f;
      std::cout << "[WORLD] Fear mode cancelled - PacMan died!" << std::endl;

      // Exit fear mode for all ghosts
      for (Ghost* ghost : ghosts) {
        if (ghost) {
          ghost->exitFearMode();
        }
      }
    }

    // ✅ ALWAYS start death animation, even on final death!
    startDeathAnimation();
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

  std::cout << "[WORLD] Fear mode activated for " << fearModeDuration
              << " seconds (level " << currentLevel << ")!" << std::endl;
  for (Ghost* ghost : ghosts) {
    if (ghost) {
      ghost->enterFearMode();
    }
  }
}

void World::updateFearMode(float deltaTime) {
  if (!fearModeActive) return;

  fearModeTimer -= deltaTime;

  // ✅ Notify ALL ghosts when fear mode is ending (last 2 seconds)
  if (fearModeTimer <= 2.0f && fearModeTimer > 0.0f) {
    for (Ghost* ghost : ghosts) {
      if (ghost) {
        ghost->setFearModeEnding(true);
      }
    }
  }

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

void World::startDeathAnimation() {
  deathAnimationActive = true;
  deathAnimationTimer = deathAnimationDuration;

  std::cout << "[WORLD] Death animation started (" << deathAnimationDuration << " seconds)" << std::endl;
}

void World::updateDeathAnimation(float deltaTime) {
  if (!deathAnimationActive) return;

  deathAnimationTimer -= deltaTime;

  if (deathAnimationTimer <= 0.0f) {
    deathAnimationActive = false;

    std::cout << "[WORLD] Death animation complete - respawning!" << std::endl;

    // Respawn everything
    respawnPacManAndGhosts();

    // ✅ Start ready state after respawn
    startReadyState();
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
  if (levelCleared) return;

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
    levelCleared = true;
    levelClearedDisplayActive = true;
    levelClearedDisplayTimer = levelClearedDisplayDuration;

    std::cout << "\n========================================" << std::endl;
    std::cout << "       🎉 LEVEL CLEARED! 🎉" << std::endl;
    std::cout << "========================================\n" << std::endl;

    if (score) {
      score->update(GameEvent::LEVEL_CLEARED);
    }

    std::cout << "[WORLD] Press ESC to quit" << std::endl;
  }
}