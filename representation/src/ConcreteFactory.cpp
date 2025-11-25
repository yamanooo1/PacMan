//
// Created by yamanooo on 11/22/25.
//

#include "representation/ConcreteFactory.h"

// Include complete types
#include "representation/EntityView.h"
#include "logic/PacMan.h"
#include "logic/Ghost.h"
#include "logic/Wall.h"
#include "logic/Coin.h"
#include "logic/Fruit.h"
#include "representation/PacManView.h"
#include "representation/GhostView.h"
#include "representation/WallView.h"
#include "representation/CoinView.h"
#include "representation/FruitView.h"

// CRITICAL: Define destructor where EntityView is complete
ConcreteFactory::~ConcreteFactory() = default;

std::unique_ptr<PacMan> ConcreteFactory::createPacMan(float x, float y) {
  auto model = std::make_unique<PacMan>(x, y);
  PacMan* modelPtr = model.get();
  auto view = std::make_unique<PacManView>(modelPtr, window);
  views.push_back(std::move(view));
  return model;
}

std::unique_ptr<Ghost> ConcreteFactory::createGhost(float x, float y) {
  auto model = std::make_unique<Ghost>(x, y);
  Ghost* modelPtr = model.get();
  auto view = std::make_unique<GhostView>(modelPtr, window);
  views.push_back(std::move(view));
  return model;
}

std::unique_ptr<Wall> ConcreteFactory::createWall(float x, float y, float w, float h) {
  auto model = std::make_unique<Wall>(x, y, w, h);
  Wall* modelPtr = model.get();
  auto view = std::make_unique<WallView>(modelPtr, window);
  views.push_back(std::move(view));
  return model;
}

std::unique_ptr<Coin> ConcreteFactory::createCoin(float x, float y) {
  auto model = std::make_unique<Coin>(x, y);
  Coin* modelPtr = model.get();
  auto view = std::make_unique<CoinView>(modelPtr, window);
  views.push_back(std::move(view));
  return model;
}

std::unique_ptr<Fruit> ConcreteFactory::createFruit(float x, float y) {
  auto model = std::make_unique<Fruit>(x, y);
  Fruit* modelPtr = model.get();
  auto view = std::make_unique<FruitView>(modelPtr, window);
  views.push_back(std::move(view));
  return model;
}

void ConcreteFactory::drawAll() {
  for (const auto& view : views) {
    view->draw();
  }
}