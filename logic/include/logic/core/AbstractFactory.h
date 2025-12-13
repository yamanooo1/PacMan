#ifndef PACMAN_ABSTRACTFACTORY_H
#define PACMAN_ABSTRACTFACTORY_H
#include <memory>

class PacMan;
class Ghost;
class Wall;
class Coin;
class Fruit;

enum class GhostType;
enum class GhostColor;

class AbstractFactory {
public:
    virtual ~AbstractFactory() = default;

    virtual std::unique_ptr<PacMan> createPacMan(float x, float y) = 0;
    virtual std::unique_ptr<Ghost> createGhost(float x, float y, GhostType type, GhostColor color, float waitTime,
                                               float speedMultiplier) = 0;

    virtual std::unique_ptr<Wall> createWall(float x, float y, float w, float h) = 0;
    virtual std::unique_ptr<Coin> createCoin(float x, float y) = 0;
    virtual std::unique_ptr<Fruit> createFruit(float x, float y) = 0;

    virtual void removeDeadViews() = 0;
};

#endif