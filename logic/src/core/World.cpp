#include "../../include/logic/core/World.h"
#include "../../include/logic/entities/Coin.h"
#include "../../include/logic/entities/Fruit.h"
#include "../../include/logic/entities/Ghost.h"
#include "../../include/logic/entities/PacMan.h"
#include "../../include/logic/entities/Wall.h"
#include "../../include/logic/systems/Lives.h"
#include "../../include/logic/systems/Score.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace logic {

/**
 * @brief Initialize world with level-scaled difficulty
 *
 * Difficulty progression:
 * - Level 1: ghosts 100% speed, fear 7.0s
 * - Level 3: ghosts 118% speed, fear 5.74s
 * - Level 5: ghosts 136% speed, fear 4.48s
 * - Level 6+: ghosts 145% speed, fear 3.22s (capped)
 */
World::World(AbstractFactory* f, int level)
    : mapWidth(0), mapHeight(0), currentLevel(level),
      ghostSpeedMultiplier(1.0f + static_cast<float>((level - 1)) * 0.09f),
      fearDurationMultiplier(1.0f - static_cast<float>((std::min(level, 6) - 1)) * 0.09f), factory(f), pacman(nullptr),
      score(nullptr), lives(nullptr), pacmanSpawnX(0.0f), pacmanSpawnY(0.0f), fearModeActive(false),
      fearModeTimer(0.0f), fearModeDuration(7.0f * fearDurationMultiplier), deathAnimationActive(false),
      deathAnimationTimer(0.0f), deathAnimationDuration(2.0f), readyStateActive(false), readyStateTimer(0.0f),
      readyStateDuration(4.7f), levelCleared(false), levelClearedDisplayActive(false), levelClearedDisplayTimer(0.0f),
      levelClearedDisplayDuration(3.0f) {}

/**
 * @brief Start ready state countdown before gameplay begins
 *
 * Displays "READY!" message for 4.7 seconds.
 */
void World::startReadyState() {
    readyStateActive = true;
    readyStateTimer = readyStateDuration;
}

void World::updateReadyState(float deltaTime) {
    if (!readyStateActive)
        return;

    readyStateTimer -= deltaTime;

    if (readyStateTimer <= 0.0f) {
        readyStateActive = false;
    }
}

void World::setMapDimensions(int width, int height) {
    mapWidth = width;
    mapHeight = height;
}

void World::addEntity(std::unique_ptr<EntityModel> entity) { entities.push_back(std::move(entity)); }

/**
 * @brief Create PacMan with proper centering and observer attachment
 *
 * Centering calculation:
 * - Map position: integer grid coordinates
 * - PacMan size: 0.8x0.8 in 1.0x1.0 cell
 * - Centered offset: (1.0 - 0.8) / 2 = 0.1
 */
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
        pacman->attach(lives->shared_from_this());
    }

    addEntity(std::move(pacmanModel));
}

/**
 * @brief Create ghost with personality, color, and spawn timing
 *
 * Wait times create staggered exits:
 * - Red: 0.0s, Cyan: 0.6s, Orange: 5.0s, Pink: 10.0s
 */
void World::createGhost(float x, float y, GhostType type, GhostColor color, float waitTime) {
    auto ghost = factory->createGhost(x, y, type, color, waitTime, ghostSpeedMultiplier);
    Ghost* ghostPtr = ghost.get();

    float centeredX = x + (1.0f - ghost->getWidth()) / 2.0f;
    float centeredY = y + (1.0f - ghost->getHeight()) / 2.0f;
    ghost->setPosition(centeredX, centeredY);

    if (score) {
        ghost->attach(score->shared_from_this());
    }

    ghostSpawnPositions.emplace_back(centeredX, centeredY);
    ghosts.push_back(ghostPtr);
    addEntity(std::move(ghost));
}

void World::createWall(float x, float y, float w, float h) {
    auto wall = factory->createWall(x, y, w, h);
    addEntity(std::move(wall));
}

/**
 * @brief Create coin with centering for visual alignment
 *
 * Coin size: 0.02x0.02, offset: (1.0 - 0.02) / 2 = 0.49
 */
void World::createCoin(float x, float y) {
    auto coin = factory->createCoin(x, y);

    float centeredX = x + (1.0f - coin->getWidth()) / 2.0f;
    float centeredY = y + (1.0f - coin->getHeight()) / 2.0f;
    coin->setPosition(centeredX, centeredY);

    if (score) {
        coin->attach(score->shared_from_this());
    }

    addEntity(std::move(coin));
}

/**
 * @brief Create fruit (power pellet) with centering
 *
 * Fruit size: 0.03x0.03, offset: (1.0 - 0.03) / 2 = 0.485
 */
void World::createFruit(float x, float y) {
    auto fruit = factory->createFruit(x, y);

    float centeredX = x + (1.0f - fruit->getWidth()) / 2.0f;
    float centeredY = y + (1.0f - fruit->getHeight()) / 2.0f;
    fruit->setPosition(centeredX, centeredY);

    if (score) {
        fruit->attach(score->shared_from_this());
    }

    addEntity(std::move(fruit));
}

/**
 * @brief Load and parse map file to populate world
 *
 * File format:
 * Line 1: "width height"
 * Lines 2+: Grid of tile characters (x=wall, .=coin, P=pacman, etc.)
 */
bool World::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
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
            auto x = static_cast<float>(col);
            auto y = static_cast<float>(row);

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
                exitPositions.emplace_back(col, row);
                break;
            default:
                break;
            }
        }
        row++;
    }

    file.close();
    return true;
}

/**
 * @brief Reset PacMan and ghosts to spawn positions after death
 *
 * Called when death animation completes.
 */
void World::respawnPacManAndGhosts() {
    if (!pacman)
        return;

    pacman->respawn();

    for (Ghost* ghost : ghosts) {
        if (ghost) {
            ghost->exitFearMode();
        }
    }

    for (size_t i = 0; i < ghosts.size(); ++i) {
        if (i < ghostSpawnPositions.size()) {
            auto [spawnX, spawnY] = ghostSpawnPositions[i];
            ghosts[i]->setPosition(spawnX, spawnY);
            ghosts[i]->setDirection(Direction::UP);
            ghosts[i]->resetSpawnFlag();
        }
    }
}

/**
 * @brief Main world update - game loop coordinator
 *
 * Update priority (exclusive states):
 * 1. Ready state → countdown
 * 2. Death animation → wait for respawn
 * 3. Level cleared → display message
 * 4. Game over → no updates
 * 5. Normal gameplay → full update cycle
 */
void World::update(float deltaTime) {
    if (!pacman)
        return;

    if (readyStateActive) {
        updateReadyState(deltaTime);
        return;
    }

    if (deathAnimationActive) {
        updateDeathAnimation(deltaTime);
        return;
    }

    if (levelClearedDisplayActive) {
        levelClearedDisplayTimer -= deltaTime;
        if (levelClearedDisplayTimer <= 0.0f) {
            levelClearedDisplayActive = false;
        }
        return;
    }

    if (lives && lives->isGameOver()) {
        return;
    }

    updatePacMan(deltaTime);
    updateGhosts(deltaTime);
    updateFearMode(deltaTime);
    checkCollisions();
    removeDeadEntities();
    checkLevelComplete();
}

/**
 * @brief Advanced PacMan movement with turn validation and wall prediction
 *
 * Features:
 * - Desired direction buffering
 * - Grid alignment validation for perpendicular turns
 * - Wall collision prediction
 * - Smooth grid snapping
 */
void World::updatePacMan(float deltaTime) const {
    auto [x, y] = pacman->getPosition();
    Direction currentDir = pacman->getDirection();
    Direction desiredDir = pacman->getDesiredDirection();

    float centerX = x + pacman->getWidth() / 2.0f;
    float centerY = y + pacman->getHeight() / 2.0f;

    int currentGridX = static_cast<int>(std::floor(centerX));
    int currentGridY = static_cast<int>(std::floor(centerY));
    float gridCenterX = static_cast<float>(currentGridX) + 0.5f;
    float gridCenterY = static_cast<float>(currentGridY) + 0.5f;

    static float prevCenterX = centerX;
    static float prevCenterY = centerY;

    float speed = pacman->getSpeed();
    float maxMovePerFrame = speed * deltaTime;
    const float centerTolerance = std::max(0.15f, maxMovePerFrame * 1.5f);

    bool atCenterX = std::abs(centerX - gridCenterX) < centerTolerance;
    bool atCenterY = std::abs(centerY - gridCenterY) < centerTolerance;

    // Check if crossed grid center this frame
    if (currentDir == Direction::LEFT || currentDir == Direction::RIGHT) {
        if ((prevCenterX < gridCenterX && centerX >= gridCenterX) ||
            (prevCenterX > gridCenterX && centerX <= gridCenterX)) {
            atCenterX = true;
        }
    }

    if (currentDir == Direction::UP || currentDir == Direction::DOWN) {
        if ((prevCenterY < gridCenterY && centerY >= gridCenterY) ||
            (prevCenterY > gridCenterY && centerY <= gridCenterY)) {
            atCenterY = true;
        }
    }

    bool atCenter = atCenterX && atCenterY;

    prevCenterX = centerX;
    prevCenterY = centerY;

    // TURN EXECUTION
    if (desiredDir != Direction::NONE && desiredDir != currentDir) {
        int testX = currentGridX;
        int testY = currentGridY;

        switch (desiredDir) {
        case Direction::UP:
            testY--;
            break;
        case Direction::DOWN:
            testY++;
            break;
        case Direction::LEFT:
            testX--;
            break;
        case Direction::RIGHT:
            testX++;
            break;
        case Direction::NONE:
            break;
        }

        if (!hasWallInGridCell(testX, testY)) {
            bool isPerpendicular = false;
            if (((currentDir == Direction::UP || currentDir == Direction::DOWN) &&
                 (desiredDir == Direction::LEFT || desiredDir == Direction::RIGHT)) ||
                ((currentDir == Direction::LEFT || currentDir == Direction::RIGHT) &&
                 (desiredDir == Direction::UP || desiredDir == Direction::DOWN))) {
                isPerpendicular = true;
            }

            bool canTurn = false;
            if (currentDir == Direction::NONE || !isPerpendicular || atCenter) {
                canTurn = true;
            }
            if (canTurn) {
                float newX = gridCenterX - pacman->getWidth() / 2.0f;
                float newY = gridCenterY - pacman->getHeight() / 2.0f;
                pacman->setPosition(newX, newY);

                pacman->setDirection(desiredDir);
                currentDir = desiredDir;
            }
        }
    }

    // WALL COLLISION PREDICTION
    int testX = currentGridX;
    int testY = currentGridY;

    switch (currentDir) {
    case Direction::UP:
        testY--;
        break;
    case Direction::DOWN:
        testY++;
        break;
    case Direction::LEFT:
        testX--;
        break;
    case Direction::RIGHT:
        testX++;
        break;
    case Direction::NONE:
        break;
    }

    if (hasWallInGridCell(testX, testY)) {
        float halfWidth = pacman->getWidth() / 2.0f;
        float halfHeight = pacman->getHeight() / 2.0f;
        bool shouldStop = false;

        switch (currentDir) {
        case Direction::LEFT:
            if (centerX <= (static_cast<float>(testX) + 1.0f) + halfWidth) {
                shouldStop = true;
            }
            break;
        case Direction::RIGHT:
            if (centerX >= static_cast<float>(testX) - halfWidth) {
                shouldStop = true;
            }
            break;
        case Direction::UP:
            if (centerY <= (static_cast<float>(testY) + 1.0f) + halfHeight) {
                shouldStop = true;
            }
            break;
        case Direction::DOWN:
            if (centerY >= static_cast<float>(testY) - halfHeight) {
                shouldStop = true;
            }
            break;
        case Direction::NONE:
            break;
        }

        if (shouldStop) {
            pacman->setDirection(Direction::NONE);
            pacman->hitWall();
        }
    }

    pacman->update(deltaTime, true);
}

bool World::isWall(float x, float y, float width, float height) const {
    for (const auto& entity : entities) {
        auto [ex, ey] = entity->getPosition();
        float ew = entity->getWidth();
        float eh = entity->getHeight();

        if (ew < 0.9f || eh < 0.9f) {
            continue;
        }

        if (x < ex + ew && x + width > ex && y < ey + eh && y + height > ey) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Fast grid-cell-based wall detection
 *
 * Primary method used by PacMan and Ghost movement.
 * Filter entities by size >= 0.9 to identify walls.
 */
bool World::hasWallInGridCell(int gridX, int gridY) const {
    for (const auto& entity : entities) {
        auto [ex, ey] = entity->getPosition();
        float ew = entity->getWidth();
        float eh = entity->getHeight();

        if (ew < 0.9f || eh < 0.9f) {
            continue;
        }

        auto wallGridX = static_cast<int>(std::floor(ex));
        auto wallGridY = static_cast<int>(std::floor(ey));

        if (wallGridX == gridX && wallGridY == gridY) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Collision detection between PacMan and all entities
 *
 * Uses AABB collision and size-based entity identification:
 * - Coin: 0.02x0.02
 * - Fruit: 0.03x0.03
 * - Ghost: 0.8x0.8
 */
void World::checkCollisions() {
    if (!pacman)
        return;

    auto [px, py] = pacman->getPosition();
    float pw = pacman->getWidth();
    float ph = pacman->getHeight();

    bool pacmanDied = false;

    for (auto& entity : entities) {
        if (entity->isDead())
            continue;
        if (entity.get() == pacman)
            continue;

        auto [ex, ey] = entity->getPosition();
        float ew = entity->getWidth();
        float eh = entity->getHeight();

        bool collision = (px < ex + ew && px + pw > ex && py < ey + eh && py + ph > ey);

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
                    collidedGhost->onEaten();

                    if (ghostIndex >= 0 && ghostIndex < ghostSpawnPositions.size()) {
                        auto [spawnX, spawnY] = ghostSpawnPositions[ghostIndex];
                        collidedGhost->setPosition(spawnX, spawnY);
                        collidedGhost->setDirection(Direction::UP);
                        collidedGhost->exitFearMode();
                        collidedGhost->respawnAfterEaten();
                    }
                } else {
                    pacmanDied = true;
                }
            } else if (ew > 0.02f && ew < 0.04f && eh > 0.02f && eh < 0.04f) {
                entity->onCollisionWithPacMan();
                activateFearMode();
            } else {
                entity->onCollisionWithPacMan();
            }
        }
    }

    if (pacmanDied && lives) {
        pacman->die();

        if (fearModeActive) {
            fearModeActive = false;
            fearModeTimer = 0.0f;

            for (Ghost* ghost : ghosts) {
                if (ghost) {
                    ghost->exitFearMode();
                }
            }
        }

        startDeathAnimation();
    }
}

/**
 * @brief Remove dead entities from world (two-phase cleanup)
 */
void World::removeDeadEntities() {
    factory->removeDeadViews();

    entities.erase(std::remove_if(entities.begin(), entities.end(),
                                  [](const std::unique_ptr<EntityModel>& entity) { return entity->isDead(); }),
                   entities.end());
}

/**
 * @brief Activate fear mode - ghosts become vulnerable
 *
 * Duration scaled by level (~7s base).
 */
void World::activateFearMode() {
    fearModeActive = true;
    fearModeTimer = fearModeDuration;

    for (Ghost* ghost : ghosts) {
        if (ghost) {
            ghost->enterFearMode();
        }
    }
}

/**
 * @brief Update fear mode timer with warning phase
 *
 * Last 2 seconds: flashing animation warning.
 */
void World::updateFearMode(float deltaTime) {
    if (!fearModeActive)
        return;

    fearModeTimer -= deltaTime;

    if (fearModeTimer <= 2.0f && fearModeTimer > 0.0f) {
        for (Ghost* ghost : ghosts) {
            if (ghost) {
                ghost->setFearModeEnding(true);
            }
        }
    }

    if (fearModeTimer <= 0.0f) {
        fearModeActive = false;

        for (Ghost* ghost : ghosts) {
            if (ghost) {
                ghost->exitFearMode();
            }
        }
    }
}

bool World::isFearModeEnding() const { return fearModeActive && fearModeTimer <= 2.0f; }

void World::startDeathAnimation() {
    deathAnimationActive = true;
    deathAnimationTimer = deathAnimationDuration;
}

/**
 * @brief Update death animation timer
 *
 * After 2.0s: respawn and start ready state.
 */
void World::updateDeathAnimation(float deltaTime) {
    if (!deathAnimationActive)
        return;

    deathAnimationTimer -= deltaTime;

    if (deathAnimationTimer <= 0.0f) {
        deathAnimationActive = false;
        respawnPacManAndGhosts();
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

std::vector<std::pair<int, int>> World::getExitPositions() const { return exitPositions; }

void World::updateGhosts(float deltaTime) {
    for (Ghost* ghost : ghosts) {
        if (ghost) {
            ghost->update(deltaTime, this, pacman);
        }
    }
}

/**
 * @brief Check if level complete (all collectables gone)
 *
 * Win condition: no coins or fruits remaining.
 */
void World::checkLevelComplete() {
    if (levelCleared)
        return;

    int remainingCollectables = 0;

    for (const auto& entity : entities) {
        if (entity->isDead())
            continue;

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

        if (score) {
            score->update(GameEvent::LEVEL_CLEARED);
        }
    }
}

} // namespace logic