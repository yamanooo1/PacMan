#ifndef PACMAN_CONCRETEFACTORY_H
#define PACMAN_CONCRETEFACTORY_H

#include "../../../../logic/include/logic/core/AbstractFactory.h"
#include "SFML/Graphics.hpp"
#include <memory>
#include <vector>

namespace logic {
class Observer;
}

namespace representation {

class EntityView;
class Camera;
class SpriteAtlas;

class ConcreteFactory : public logic::AbstractFactory {
private:
    sf::RenderWindow& window;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<SpriteAtlas> spriteAtlas;
    std::vector<std::shared_ptr<EntityView>> views;
    std::shared_ptr<logic::Observer> soundObserver;

public:
    explicit ConcreteFactory(sf::RenderWindow& win, std::shared_ptr<Camera> cam);
    ~ConcreteFactory() override;

    bool loadSprites(const std::string& filepath);

    void setSoundObserver(const std::shared_ptr<logic::Observer>& observer) { soundObserver = observer; }

    std::unique_ptr<logic::PacMan> createPacMan(float x, float y) override;
    std::unique_ptr<logic::Ghost> createGhost(float x, float y, logic::GhostType type, logic::GhostColor color, float waitTime,
                                       float speedMultiplier) override;
    std::unique_ptr<logic::Wall> createWall(float x, float y, float w, float h) override;
    std::unique_ptr<logic::Coin> createCoin(float x, float y) override;
    std::unique_ptr<logic::Fruit> createFruit(float x, float y) override;

    void removeDeadViews() override;

    void drawAll();
    void updateAll();
};

} // namespace representation

#endif