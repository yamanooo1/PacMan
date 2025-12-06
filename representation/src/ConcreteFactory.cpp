//
// ConcreteFactory.cpp - UPDATED with shared_ptr<Camera>
//

#include "representation/ConcreteFactory.h"
#include "representation/SpriteAtlas.h"

// Include complete types
#include <iostream>
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
#include "logic/Stopwatch.h"

ConcreteFactory::ConcreteFactory(sf::RenderWindow& win, std::shared_ptr<Camera> cam)  // ✅ Reference
    : window(win)  // ✅ Initialize reference
    , camera(cam)
    , spriteAtlas(std::make_shared<SpriteAtlas>())
{
}

ConcreteFactory::~ConcreteFactory() = default;

bool ConcreteFactory::loadSprites(const std::string& filepath) {
    if (!spriteAtlas) {
        std::cerr << "[FACTORY] SpriteAtlas not initialized!" << std::endl;
        return false;
    }

    bool success = spriteAtlas->loadFromFile(filepath);
    if (success) {
        std::cout << "[FACTORY] Sprites loaded successfully" << std::endl;
    } else {
        std::cerr << "[FACTORY] Failed to load sprites from: " << filepath << std::endl;
    }

    return success;
}

std::unique_ptr<PacMan> ConcreteFactory::createPacMan(float x, float y) {
  auto model = std::make_unique<PacMan>(x, y);
  PacMan* modelPtr = model.get();

  auto view = std::make_unique<PacManView>(modelPtr, window, camera, spriteAtlas);

  views.push_back(std::move(view));
  return model;
}

std::unique_ptr<Ghost> ConcreteFactory::createGhost(float x, float y, GhostType type, GhostColor color, float waitTime, float speedMultiplier) {
  auto model = std::make_unique<Ghost>(x, y, type, color, waitTime, speedMultiplier);
  Ghost* modelPtr = model.get();
  auto view = std::make_unique<GhostView>(modelPtr, window, camera, spriteAtlas);

  switch (color) {
  case GhostColor::RED:
    view->setColor(sf::Color::Red);
    view->setSpriteType(SpriteGhostType::RED);
    break;
  case GhostColor::PINK:
    view->setColor(sf::Color::Magenta);
    view->setSpriteType(SpriteGhostType::PINK);
    break;
  case GhostColor::CYAN:
    view->setColor(sf::Color::Cyan);
    view->setSpriteType(SpriteGhostType::CYAN);
    break;
  case GhostColor::ORANGE:
    view->setColor(sf::Color(255, 165, 0));
    view->setSpriteType(SpriteGhostType::ORANGE);
    break;
  }

  views.push_back(std::move(view));
  return model;
}

std::unique_ptr<Wall> ConcreteFactory::createWall(float x, float y, float w, float h) {
  auto model = std::make_unique<Wall>(x, y, w, h);
  Wall* modelPtr = model.get();

  auto view = std::make_unique<WallView>(modelPtr, window, camera, spriteAtlas);

  views.push_back(std::move(view));
  return model;
}

std::unique_ptr<Coin> ConcreteFactory::createCoin(float x, float y) {
  auto model = std::make_unique<Coin>(x, y);
  Coin* modelPtr = model.get();

  auto view = std::make_unique<CoinView>(modelPtr, window, camera, spriteAtlas);

  views.push_back(std::move(view));
  return model;
}

std::unique_ptr<Fruit> ConcreteFactory::createFruit(float x, float y) {
  auto model = std::make_unique<Fruit>(x, y);
  Fruit* modelPtr = model.get();

  auto view = std::make_unique<FruitView>(modelPtr, window, camera, spriteAtlas);

  views.push_back(std::move(view));
  return model;
}

void ConcreteFactory::drawAll() {
  for (const auto& view : views) {
    view->draw();
  }
}

void ConcreteFactory::removeDeadViews() {
  views.erase(
    std::remove_if(views.begin(), views.end(),
      [](const std::unique_ptr<EntityView>& view) {
        return view->getModel()->isDead();
      }),
    views.end()
  );
}

void ConcreteFactory::updateAll() {
  Stopwatch& stopwatch = Stopwatch::getInstance();
  float deltaTime = stopwatch.getDeltaTime();

  for (const auto& view : views) {
    view->updateAnimation(deltaTime);
  }
}