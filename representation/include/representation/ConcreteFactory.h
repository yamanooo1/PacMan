//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_CONCRETEFACTORY_H
#define PACMAN_CONCRETEFACTORY_H
#include "logic/AbstractFactory.h"
#include "SFML/Graphics.hpp"
#include <vector>
#include <memory>

class EntityView;

class ConcreteFactory: public AbstractFactory{
private:
  sf::RenderWindow* window;
  std::vector<std::unique_ptr<EntityView>> views;
public:
  explicit ConcreteFactory(sf::RenderWindow* win);

  ~ConcreteFactory() override;

  // Return unique_ptrs
  std::unique_ptr<PacMan> createPacMan(float x, float y) override;
  std::unique_ptr<Ghost> createGhost(float x, float y) override;
  std::unique_ptr<Wall> createWall(float x, float y, float w, float h) override;
  std::unique_ptr<Coin> createCoin(float x, float y) override;
  std::unique_ptr<Fruit> createFruit(float x, float y) override;

  void drawAll();
};


#endif //PACMAN_CONCRETEFACTORY_H