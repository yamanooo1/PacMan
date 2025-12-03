//
// ConcreteFactory.h - COMPLETE VERSION
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
  sf::RenderWindow* window;
  Camera* camera;
  std::shared_ptr<SpriteAtlas> spriteAtlas;
  std::vector<std::unique_ptr<EntityView>> views;

public:
  explicit ConcreteFactory(sf::RenderWindow* win, Camera* cam);
  ~ConcreteFactory() override;

  // Load sprites from file
  bool loadSprites(const std::string& filepath);

  // Factory methods (from AbstractFactory)
  std::unique_ptr<PacMan> createPacMan(float x, float y) override;
  std::unique_ptr<Ghost> createGhost(float x, float y, GhostType type, GhostColor color, float waitTime) override;  // ✅ UPDATED
  std::unique_ptr<Wall> createWall(float x, float y, float w, float h) override;
  std::unique_ptr<Coin> createCoin(float x, float y) override;
  std::unique_ptr<Fruit> createFruit(float x, float y) override;

  void removeDeadViews() override;

  // Rendering method (NOT in AbstractFactory)
  void drawAll();

  // Add this method declaration:
  void updateAll();
};

#endif //PACMAN_CONCRETEFACTORY_H