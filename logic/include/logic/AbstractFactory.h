//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_ABSTRACTFACTORY_H
#define PACMAN_ABSTRACTFACTORY_H
#include <memory>

class PacMan;
class Ghost;
class Wall;
class Coin;
class Fruit;

class AbstractFactory {
public:
  virtual ~AbstractFactory() = default;

  // Return unique_ptrs - transfer ownership to caller (World)
  virtual std::unique_ptr<PacMan> createPacMan(float x, float y) = 0;
  virtual std::unique_ptr<Ghost> createGhost(float x, float y) = 0;
  virtual std::unique_ptr<Wall> createWall(float x, float y, float w, float h) = 0;
  virtual std::unique_ptr<Coin> createCoin(float x, float y) = 0;
  virtual std::unique_ptr<Fruit> createFruit(float x, float y) = 0;

};


#endif //PACMAN_ABSTRACTFACTORY_H