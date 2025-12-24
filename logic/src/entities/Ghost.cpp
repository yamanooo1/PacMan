#include "../../include/logic/entities/Ghost.h"
#include "../../include/logic/core/World.h"
#include "../../include/logic/entities/PacMan.h"
#include "../../include/logic/utils/Random.h"
#include <cmath>
#include <vector>

namespace logic {

/**
 * @brief Initialize ghost with personality, appearance, and difficulty scaling
 *
 * Size: 0.8x0.8 matches PacMan for fair collision detection
 *
 * Speed calculation example (level 5):
 * - speedMultiplier = 1.0 + (5-1) * 0.09 = 1.36
 * - normalSpeed = 2.5 * 1.36 = 3.4 units/sec
 * - fearSpeed = 1.5 (constant, slower than PacMan's 4.0)
 */
Ghost::Ghost(float x, float y, GhostType t, GhostColor c, float waitTime, float speedMultiplier)
    : EntityModel(x, y, 0.8f, 0.8f), type(t), color(c), state(GhostState::WAITING), waitTimer(waitTime),
      originalWaitTime(waitTime), speed(2.5f * speedMultiplier), normalSpeed(2.5f * speedMultiplier), fearSpeed(1.5f),
      lastDecisionGridX(-999), lastDecisionGridY(-999), hasLeftSpawn(false), fearModeEnding(false),
      shouldEnterFearWhenLeaving(false) {
    setDirection(Direction::UP);
}

bool Ghost::isInFearMode() const { return state == GhostState::FEAR; }

bool Ghost::shouldShowFearMode() const { return state == GhostState::FEAR || shouldEnterFearWhenLeaving; }

void Ghost::resetSpawnFlag() {
    hasLeftSpawn = false;
    state = GhostState::WAITING;
    waitTimer = originalWaitTime;
}

void Ghost::respawnAfterEaten() {
    hasLeftSpawn = false;
    state = GhostState::EXITING;
    waitTimer = -1.0f;
}

void Ghost::onEaten() { notify(GameEvent::GHOST_EATEN); }

bool Ghost::isInSpawnArea(int gridX, int gridY) { return (gridX >= 7 && gridX <= 11 && gridY >= 8 && gridY <= 10); }

int Ghost::manhattanDistance(int x1, int y1, int x2, int y2) { return std::abs(x1 - x2) + std::abs(y1 - y2); }

/**
 * @brief Main ghost update - state machine and movement
 *
 * State machine transitions:
 * WAITING: timer countdown → EXITING when timer expires
 * EXITING: navigate to exit → CHASING/FEAR when reaching 'w' marker
 * CHASING: hunt PacMan → FEAR when fruit collected
 * FEAR: run away → CHASING when fear timer expires
 *
 * Movement system:
 * 1. Check if at intersection (2+ viable directions)
 * 2. If yes and grid position changed → make decision
 * 3. Move smoothly toward next cell
 * 4. If wall ahead → stop at cell center
 */
void Ghost::update(float deltaTime, World* world, const PacMan* pacman) {
    if (!world)
        return;

    // WAITING state: countdown timer in spawn area
    if (state == GhostState::WAITING) {
        waitTimer -= deltaTime;
        if (waitTimer <= 0.0f) {
            state = GhostState::EXITING;

            auto [x, y] = getPosition();
            float centerX = x + getWidth() / 2.0f;
            float centerY = y + getHeight() / 2.0f;
            int gridX = static_cast<int>(std::floor(centerX));
            int gridY = static_cast<int>(std::floor(centerY));

            Direction exitDir = chooseDirectionToExit(gridX, gridY, world);
            if (exitDir != Direction::NONE) {
                setDirection(exitDir);
                lastDecisionGridX = gridX;
                lastDecisionGridY = gridY;
            }
        }
        return; // No movement while waiting
    }

    // EXITING state: navigate to exit marker ('w' in map)
    if (state == GhostState::EXITING) {
        auto [x, y] = getPosition();
        float centerX = x + getWidth() / 2.0f;
        float centerY = y + getHeight() / 2.0f;
        int gridX = static_cast<int>(std::floor(centerX));
        int gridY = static_cast<int>(std::floor(centerY));

        // Check if reached exit position
        if (world->isExitPosition(gridX, gridY)) {
            // Transition to appropriate state
            if (shouldEnterFearWhenLeaving) {
                state = GhostState::FEAR;
                speed = fearSpeed;
                shouldEnterFearWhenLeaving = false;
            } else {
                state = GhostState::CHASING;
            }

            hasLeftSpawn = true; // Prevent re-entry

            // Choose initial hunting direction
            Direction chaseDir = chooseNextDirection(gridX, gridY, world, pacman);
            if (chaseDir != Direction::NONE) {
                setDirection(chaseDir);
                lastDecisionGridX = gridX;
                lastDecisionGridY = gridY;
            }
        }
    }

    // Movement logic (applies to EXITING, CHASING, and FEAR states)
    auto [x, y] = getPosition();
    Direction currentDir = getDirection();

    if (currentDir == Direction::NONE)
        return;

    // Calculate current and next grid positions
    float centerX = x + getWidth() / 2.0f;
    float centerY = y + getHeight() / 2.0f;
    int gridX = static_cast<int>(std::floor(centerX));
    int gridY = static_cast<int>(std::floor(centerY));

    int nextGridX = gridX;
    int nextGridY = gridY;
    switch (currentDir) {
    case Direction::UP:
        nextGridY--;
        break;
    case Direction::DOWN:
        nextGridY++;
        break;
    case Direction::LEFT:
        nextGridX--;
        break;
    case Direction::RIGHT:
        nextGridX++;
        break;
    case Direction::NONE:
        break;
    }
    bool wallAhead = world->hasWallInGridCell(nextGridX, nextGridY);

    // Check if aligned with grid center for decision making
    float gridCenterX = static_cast<float>(gridX) + 0.5f;
    float gridCenterY = static_cast<float>(gridY) + 0.5f;
    constexpr float centerTolerance = 0.1f;
    bool atCenterX = std::abs(centerX - gridCenterX) < centerTolerance;
    bool atCenterY = std::abs(centerY - gridCenterY) < centerTolerance;
    bool atCenter = atCenterX && atCenterY;

    // Decision point: at center of new grid cell?
    bool needsDecision = atCenter && (gridX != lastDecisionGridX || gridY != lastDecisionGridY);
    bool canMakeDecision = isAtIntersection(gridX, gridY, world) || wallAhead;

    // Make new direction decision at intersections or when blocked
    if (needsDecision && canMakeDecision) {
        Direction newDir;

        if (state == GhostState::EXITING) {
            newDir = chooseDirectionToExit(gridX, gridY, world);
        } else {
            newDir = chooseNextDirection(gridX, gridY, world, pacman);
        }

        if (newDir != Direction::NONE) {
            setDirection(newDir);
            lastDecisionGridX = gridX;
            lastDecisionGridY = gridY;
            currentDir = newDir;

            // Recalculate wall ahead for new direction
            nextGridX = gridX;
            nextGridY = gridY;
            switch (currentDir) {
            case Direction::UP:
                nextGridY--;
                break;
            case Direction::DOWN:
                nextGridY++;
                break;
            case Direction::LEFT:
                nextGridX--;
                break;
            case Direction::RIGHT:
                nextGridX++;
                break;
            case Direction::NONE:
                break;
            }
            wallAhead = world->hasWallInGridCell(nextGridX, nextGridY);
        }
    }

    // Execute movement if path is clear
    if (!wallAhead) {
        float moveDistance = speed * deltaTime;
        float newX = x;
        float newY = y;

        switch (currentDir) {
        case Direction::UP:
            newY -= moveDistance;
            break;
        case Direction::DOWN:
            newY += moveDistance;
            break;
        case Direction::LEFT:
            newX -= moveDistance;
            break;
        case Direction::RIGHT:
            newX += moveDistance;
            break;
        case Direction::NONE:
            break;
        }

        setPosition(newX, newY);
    } else {
        // Wall ahead: smoothly move to cell center and stop
        float targetCenterX = static_cast<float>(gridX) + 0.5f;
        float targetCenterY = static_cast<float>(gridY) + 0.5f;

        bool atTargetCenter = (std::abs(centerX - targetCenterX) < 0.01f && std::abs(centerY - targetCenterY) < 0.01f);

        if (!atTargetCenter) {
            float moveDistance = speed * deltaTime;
            float newCenterX = centerX;
            float newCenterY = centerY;

            // Move toward center
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

/**
 * @brief Find path toward nearest exit marker
 *
 * Used during EXITING state to leave spawn area.
 *
 * Algorithm:
 * 1. Get all exit positions ('w' markers) from world
 * 2. Find nearest exit using manhattan distance
 * 3. Choose direction that minimizes distance to nearest exit
 * 4. Break ties randomly for unpredictability
 */
Direction Ghost::chooseDirectionToExit(int gridX, int gridY, const World* world) const {
    auto exitPositions = world->getExitPositions();

    if (exitPositions.empty()) {
        return Direction::UP; // Fallback if no exits defined
    }

    // Find nearest exit marker
    int nearestDist = 999999;
    int targetX = exitPositions[0].first;
    int targetY = exitPositions[0].second;

    for (const auto& [ex, ey] : exitPositions) {
        int dist = manhattanDistance(gridX, gridY, ex, ey);
        if (dist < nearestDist) {
            nearestDist = dist;
            targetX = ex;
            targetY = ey;
        }
    }

    auto viable = getViableDirections(gridX, gridY, world);

    if (viable.empty())
        return getDirection();
    if (viable.size() == 1)
        return viable[0];

    // Find direction(s) that minimize distance to exit
    std::vector<Direction> bestDirections;
    int bestDistance = 999999;

    for (Direction dir : viable) {
        int nextGridX = gridX;
        int nextGridY = gridY;

        switch (dir) {
        case Direction::UP:
            nextGridY--;
            break;
        case Direction::DOWN:
            nextGridY++;
            break;
        case Direction::LEFT:
            nextGridX--;
            break;
        case Direction::RIGHT:
            nextGridX++;
            break;
        case Direction::NONE:
            break;
        }

        int distance = manhattanDistance(nextGridX, nextGridY, targetX, targetY);

        if (distance < bestDistance) {
            bestDistance = distance;
            bestDirections.clear();
            bestDirections.push_back(dir);
        } else if (distance == bestDistance) {
            bestDirections.push_back(dir);
        }
    }

    if (bestDirections.empty())
        return viable[0];
    if (bestDirections.size() == 1)
        return bestDirections[0];

    // Random tie-breaking for natural movement
    Random& random = Random::getInstance();
    int index = random.getInt(0, bestDirections.size() - 1);
    return bestDirections[index];
}

/**
 * @brief Get all valid movement directions at current position
 *
 * Validity rules:
 * 1. No walls in that direction
 * 2. No re-entering spawn area (once left)
 * 3. Prefer not turning back (opposite direction)
 * 4. Allow opposite if it's the only option (forced turn-around)
 */
std::vector<Direction> Ghost::getViableDirections(int gridX, int gridY, const World* world) const {
    std::vector<Direction> viable;
    Direction current = getDirection();

    // Calculate opposite direction (don't allow 180-degree turns unless forced)
    Direction opposite = Direction::NONE;
    if (current == Direction::UP)
        opposite = Direction::DOWN;
    else if (current == Direction::DOWN)
        opposite = Direction::UP;
    else if (current == Direction::LEFT)
        opposite = Direction::RIGHT;
    else if (current == Direction::RIGHT)
        opposite = Direction::LEFT;

    std::vector<Direction> allDirections = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};

    // First pass: add all valid directions except opposite
    for (Direction dir : allDirections) {
        if (dir == opposite)
            continue; // Skip opposite for now

        int nextGridX = gridX;
        int nextGridY = gridY;
        switch (dir) {
        case Direction::UP:
            nextGridY--;
            break;
        case Direction::DOWN:
            nextGridY++;
            break;
        case Direction::LEFT:
            nextGridX--;
            break;
        case Direction::RIGHT:
            nextGridX++;
            break;
        case Direction::NONE:
            break;
        }

        // Check validity
        if (world->hasWallInGridCell(nextGridX, nextGridY))
            continue; // Wall blocks path
        if (hasLeftSpawn && isInSpawnArea(nextGridX, nextGridY))
            continue; // Can't re-enter spawn

        viable.push_back(dir);
    }

    // Second pass: add opposite direction if allowed and needed
    if (viable.size() >= 2 && opposite != Direction::NONE) {
        // Don't allow opposite if alternatives exist (prevents back-and-forth)
        // But check validity anyway for completeness
        int nextGridX = gridX;
        int nextGridY = gridY;
        switch (opposite) {
        case Direction::UP:
            nextGridY--;
            break;
        case Direction::DOWN:
            nextGridY++;
            break;
        case Direction::LEFT:
            nextGridX--;
            break;
        case Direction::RIGHT:
            nextGridX++;
            break;
        case Direction::NONE:
            break;
        }

        bool hasWall = world->hasWallInGridCell(nextGridX, nextGridY);
        bool wouldEnterSpawn = hasLeftSpawn && isInSpawnArea(nextGridX, nextGridY);

        if (!hasWall && !wouldEnterSpawn) {
            viable.push_back(opposite); // Allow opposite as last resort
        }
    }

    // Edge case: if no options, allow opposite direction even alone
    if (viable.empty() && opposite != Direction::NONE) {
        int nextGridX = gridX;
        int nextGridY = gridY;
        switch (opposite) {
        case Direction::UP:
            nextGridY--;
            break;
        case Direction::DOWN:
            nextGridY++;
            break;
        case Direction::LEFT:
            nextGridX--;
            break;
        case Direction::RIGHT:
            nextGridX++;
            break;
        case Direction::NONE:
            break;
        }

        bool hasWall = world->hasWallInGridCell(nextGridX, nextGridY);
        bool wouldEnterSpawn = hasLeftSpawn && isInSpawnArea(nextGridX, nextGridY);

        if (!hasWall && !wouldEnterSpawn) {
            viable.push_back(opposite); // Forced turn-around
        }
    }

    return viable;
}

/**
 * @brief Check if at intersection (decision point)
 *
 * Intersection = 2+ viable directions available
 * Ghosts only reconsider direction at intersections for smooth movement.
 */
bool Ghost::isAtIntersection(int gridX, int gridY, World* world) const {
    auto viable = getViableDirections(gridX, gridY, world);
    return viable.size() >= 2;
}

/**
 * @brief AI decision making based on ghost personality
 *
 * Three strategies:
 *
 * 1. RANDOM (Orange ghost):
 *    - 50% chance: pick random direction
 *    - 50% chance: continue current direction
 *    - Unpredictable, easier to evade
 *
 * 2. CHASER (Red ghost):
 *    - Target PacMan's current position directly
 *    - Minimize manhattan distance
 *    - Aggressive, relentless pursuit
 *
 * 3. AMBUSHER (Pink/Cyan ghosts):
 *    - Target 4 cells ahead of PacMan
 *    - If already closer to target than PacMan → chase PacMan directly
 *    - Strategic positioning for ambush
 *
 * 4. FEAR mode (all ghosts when blue):
 *    - Maximize distance from PacMan
 *    - Run away instead of chase
 *    - Vulnerable state
 */
Direction Ghost::chooseNextDirection(int gridX, int gridY, const World* world, const PacMan* pacman) const {
    auto viable = getViableDirections(gridX, gridY, world);

    if (viable.empty())
        return getDirection(); // Stuck, keep trying current direction
    if (viable.size() == 1)
        return viable[0]; // Only one option, take it

    Random& random = Random::getInstance();

    // RANDOM ghost: coin flip between random and current direction
    if (type == GhostType::RANDOM) {
        Direction current = getDirection();

        bool currentViable = false;
        for (Direction dir : viable) {
            if (dir == current) {
                currentViable = true;
                break;
            }
        }

        if (random.getBool()) {
            // Random direction
            int index = random.getInt(0, viable.size() - 1);
            return viable[index];
        } else {
            // Prefer current direction if valid
            if (currentViable) {
                return current;
            } else {
                int index = random.getInt(0, viable.size() - 1);
                return viable[index];
            }
        }
    }

    if (!pacman) {
        // No PacMan to target, pick random
        int index = random.getInt(0, viable.size() - 1);
        return viable[index];
    }

    // Calculate PacMan's grid position
    auto [px, py] = pacman->getPosition();
    float centerX = px + pacman->getWidth() / 2.0f;
    float centerY = py + pacman->getHeight() / 2.0f;
    int pacmanGridX = static_cast<int>(std::floor(centerX));
    int pacmanGridY = static_cast<int>(std::floor(centerY));

    int targetX, targetY;

    // Determine target based on state and type
    if (state == GhostState::FEAR || type == GhostType::CHASER) {
        // Direct targeting
        targetX = pacmanGridX;
        targetY = pacmanGridY;
    } else {
        // AMBUSHER: target ahead of PacMan
        Direction pacmanDir = pacman->getDirection();

        if (pacmanDir == Direction::NONE) {
            // PacMan not moving, target current position
            targetX = pacmanGridX;
            targetY = pacmanGridY;
        } else {
            // Look ahead 4 cells in PacMan's direction
            int lookAhead = 4;
            int lookAheadX = pacmanGridX;
            int lookAheadY = pacmanGridY;

            switch (pacmanDir) {
            case Direction::UP:
                lookAheadY -= lookAhead;
                break;
            case Direction::DOWN:
                lookAheadY += lookAhead;
                break;
            case Direction::LEFT:
                lookAheadX -= lookAhead;
                break;
            case Direction::RIGHT:
                lookAheadX += lookAhead;
                break;
            case Direction::NONE:
                break;
            }

            // Clamp to map boundaries
            int mapWidth = 21;
            int mapHeight = 21;
            lookAheadX = std::max(0, std::min(mapWidth - 1, lookAheadX));
            lookAheadY = std::max(0, std::min(mapHeight - 1, lookAheadY));

            // If ghost is already closer to ambush point than PacMan, switch to direct chase
            int ghostToTarget = manhattanDistance(gridX, gridY, lookAheadX, lookAheadY);
            int pacmanToTarget = manhattanDistance(pacmanGridX, pacmanGridY, lookAheadX, lookAheadY);

            if (ghostToTarget < pacmanToTarget) {
                // Already at ambush point, chase PacMan directly
                targetX = pacmanGridX;
                targetY = pacmanGridY;
            } else {
                // Move to ambush position
                targetX = lookAheadX;
                targetY = lookAheadY;
            }
        }
    }

    // Find direction(s) that optimize distance to target
    bool shouldMaximize = (state == GhostState::FEAR); // Flee instead of chase

    std::vector<Direction> bestDirections;
    int bestDistance = shouldMaximize ? -1 : 999999;

    for (Direction dir : viable) {
        int nextGridX = gridX;
        int nextGridY = gridY;

        switch (dir) {
        case Direction::UP:
            nextGridY--;
            break;
        case Direction::DOWN:
            nextGridY++;
            break;
        case Direction::LEFT:
            nextGridX--;
            break;
        case Direction::RIGHT:
            nextGridX++;
            break;
        case Direction::NONE:
            break;
        }

        int distance = manhattanDistance(nextGridX, nextGridY, targetX, targetY);

        if (shouldMaximize) {
            // FEAR mode: maximize distance (run away)
            if (distance > bestDistance) {
                bestDistance = distance;
                bestDirections.clear();
                bestDirections.push_back(dir);
            } else if (distance == bestDistance) {
                bestDirections.push_back(dir);
            }
        } else {
            // Normal mode: minimize distance (chase)
            if (distance < bestDistance) {
                bestDistance = distance;
                bestDirections.clear();
                bestDirections.push_back(dir);
            } else if (distance == bestDistance) {
                bestDirections.push_back(dir);
            }
        }
    }

    if (bestDirections.empty()) {
        // Fallback to random
        int index = random.getInt(0, viable.size() - 1);
        return viable[index];
    }

    // Random tie-breaking among equally good options
    Direction chosen =
        bestDirections.size() == 1 ? bestDirections[0] : bestDirections[random.getInt(0, bestDirections.size() - 1)];

    return chosen;
}

/**
 * @brief Activate fear mode - ghosts become vulnerable and flee
 *
 * Behavior changes:
 * - Color: Normal → Blue (flashing in last 2 seconds)
 * - AI: Chase → Flee (maximize distance from PacMan)
 * - Speed: normalSpeed → fearSpeed (slower)
 * - Collision: Kill PacMan → Get eaten (+200 points)
 *
 * Special cases:
 * - If WAITING/EXITING: queue fear mode for after exit
 * - If already FEAR: this is a refresh (new fruit collected)
 *
 * Immediate turn-around:
 * - Reverses direction instantly to prevent catching player off-guard
 * - Creates dramatic chase reversal moment
 */
void Ghost::enterFearMode() {
    fearModeEnding = false;

    // Special case: not yet out of spawn
    if (state == GhostState::WAITING || state == GhostState::EXITING) {
        shouldEnterFearWhenLeaving = true; // Activate fear when reaching exit
        return;
    }

    state = GhostState::FEAR;
    speed = fearSpeed;

    // Immediate 180-degree turn for dramatic effect
    Direction current = getDirection();
    Direction opposite = Direction::NONE;

    if (current == Direction::UP)
        opposite = Direction::DOWN;
    else if (current == Direction::DOWN)
        opposite = Direction::UP;
    else if (current == Direction::LEFT)
        opposite = Direction::RIGHT;
    else if (current == Direction::RIGHT)
        opposite = Direction::LEFT;

    if (opposite != Direction::NONE) {
        setDirection(opposite);
    }

    // Reset decision cache to force immediate recalculation
    lastDecisionGridX = -999;
    lastDecisionGridY = -999;
}

/**
 * @brief Exit fear mode - return to normal hunting
 *
 * Called when fear timer expires in World::updateFearMode()
 * Restores normal ghost behavior and appearance.
 */
void Ghost::exitFearMode() {
    if (state != GhostState::FEAR) {
        shouldEnterFearWhenLeaving = false;
        return;
    }

    state = GhostState::CHASING;
    speed = normalSpeed;
    fearModeEnding = false;
    shouldEnterFearWhenLeaving = false;

    // Reset decision cache for new targeting
    lastDecisionGridX = -999;
    lastDecisionGridY = -999;
}

} // namespace logic