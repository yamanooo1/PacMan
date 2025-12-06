//
// ConcreteFactory.h - UPDATED with window reference
//

#ifndef PACMAN_CONCRETEFACTORY_H
#define PACMAN_CONCRETEFACTORY_H

#include "logic/AbstractFactory.h"
#include "SFML/Graphics.hpp"
#include <vector>
#include <memory>

class EntityView;
class Camera;
class SpriteAtlas;

class ConcreteFactory: public AbstractFactory {
private:
  sf::RenderWindow& window;  // ✅ CHANGED to reference
  std::shared_ptr<Camera> camera;
  std::shared_ptr<SpriteAtlas> spriteAtlas;
  std::vector<std::unique_ptr<EntityView>> views;

public:
  explicit ConcreteFactory(sf::RenderWindow& win, std::shared_ptr<Camera> cam);  // ✅ Reference
  ~ConcreteFactory() override;

  bool loadSprites(const std::string& filepath);

  std::unique_ptr<PacMan> createPacMan(float x, float y) override;
  std::unique_ptr<Ghost> createGhost(float x, float y, GhostType type, GhostColor color, float waitTime, float speedMultiplier = 1.0f) override;
  std::unique_ptr<Wall> createWall(float x, float y, float w, float h) override;
  std::unique_ptr<Coin> createCoin(float x, float y) override;
  std::unique_ptr<Fruit> createFruit(float x, float y) override;

  void removeDeadViews() override;

  void drawAll();
  void updateAll();
};

#endif //PACMAN_CONCRETEFACTORY_H