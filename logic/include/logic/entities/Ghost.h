#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H
#include "../core/EntityModel.h"
#include <vector>

class World;
class PacMan;

enum class GhostColor { RED, PINK, CYAN, ORANGE };

enum class GhostType { RANDOM, AMBUSHER, CHASER };

enum class GhostState { WAITING, EXITING, CHASING, FEAR };

class Ghost : public EntityModel {
private:
    GhostType type;
    GhostColor color;
    GhostState state;
    float waitTimer;
    float originalWaitTime;
    float speed;
    float normalSpeed;
    float fearSpeed;

    int lastDecisionGridX;
    int lastDecisionGridY;

    bool hasLeftSpawn;
    bool fearModeEnding;
    bool shouldEnterFearWhenLeaving;

    std::vector<Direction> getViableDirections(int gridX, int gridY, const World* world) const;
    bool isAtIntersection(int gridX, int gridY, World* world) const;
    Direction chooseNextDirection(int gridX, int gridY, const World* world, const PacMan* pacman) const;
    Direction chooseDirectionToExit(int gridX, int gridY, const World* world) const;
    [[nodiscard]] static int manhattanDistance(int x1, int y1, int x2, int y2);
    [[nodiscard]] static bool isInSpawnArea(int gridX, int gridY);

public:
    Ghost(float x, float y, GhostType t, GhostColor c, float waitTime, float speedMultiplier = 1.0f);
    ~Ghost() override = default;

    [[nodiscard]] GhostType getType() const { return type; }
    [[nodiscard]] GhostState getState() const { return state; }
    [[nodiscard]] GhostColor getColor() const { return color; }

    void enterFearMode();
    void exitFearMode();
    [[nodiscard]] bool isInFearMode() const;
    [[nodiscard]] bool shouldShowFearMode() const;

    void resetSpawnFlag();
    void respawnAfterEaten();
    void onEaten();

    void update(float deltaTime, World* world, const PacMan* pacman);

    [[nodiscard]] bool isFearModeEnding() const { return fearModeEnding; }
    void setFearModeEnding(bool ending) { fearModeEnding = ending; }
};

#endif