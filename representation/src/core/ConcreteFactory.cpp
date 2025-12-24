#include "../../include/representation/core/ConcreteFactory.h"
#include "../../../logic/include/logic/entities/Coin.h"
#include "../../../logic/include/logic/entities/Fruit.h"
#include "../../../logic/include/logic/entities/Ghost.h"
#include "../../../logic/include/logic/entities/PacMan.h"
#include "../../../logic/include/logic/entities/Wall.h"
#include "../../../logic/include/logic/utils/Stopwatch.h"
#include "../../include/representation/core/SpriteAtlas.h"
#include "../../include/representation/entities/CoinView.h"
#include "../../include/representation/entities/EntityView.h"
#include "../../include/representation/entities/FruitView.h"
#include "../../include/representation/entities/GhostView.h"
#include "../../include/representation/entities/PacManView.h"
#include "../../include/representation/entities/WallView.h"
#include <algorithm>

namespace representation {

using logic::PacMan;
using logic::Ghost;
using logic::Wall;
using logic::Coin;
using logic::Fruit;
using logic::GhostType;
using logic::GhostColor;
using logic::Stopwatch;

ConcreteFactory::ConcreteFactory(sf::RenderWindow& win, std::shared_ptr<Camera> cam)
    : window(win), camera(std::move(cam)), spriteAtlas(std::make_shared<SpriteAtlas>()), soundObserver(nullptr) {}

ConcreteFactory::~ConcreteFactory() = default;

bool ConcreteFactory::loadSprites(const std::string& filepath) {
    if (!spriteAtlas) {
        return false;
    }

    return spriteAtlas->loadFromFile(filepath);
}

std::unique_ptr<logic::PacMan> ConcreteFactory::createPacMan(float x, float y) {
    auto model = std::make_unique<logic::PacMan>(x, y);
    logic::PacMan* modelPtr = model.get();

    auto view = std::make_shared<PacManView>(modelPtr, window, camera, spriteAtlas);
    modelPtr->attach(view);

    if (soundObserver) {
        modelPtr->attach(soundObserver);
    }

    views.push_back(view);
    return model;
}

std::unique_ptr<logic::Ghost> ConcreteFactory::createGhost(float x, float y, logic::GhostType type, logic::GhostColor color, float waitTime,
                                                    float speedMultiplier) {
    auto model = std::make_unique<logic::Ghost>(x, y, type, color, waitTime, speedMultiplier);
    logic::Ghost* modelPtr = model.get();

    auto view = std::make_shared<GhostView>(modelPtr, window, camera, spriteAtlas);

    switch (color) {
    case logic::GhostColor::RED:
        view->setColor(sf::Color::Red);
        view->setSpriteType(SpriteGhostType::RED);
        break;
    case logic::GhostColor::PINK:
        view->setColor(sf::Color::Magenta);
        view->setSpriteType(SpriteGhostType::PINK);
        break;
    case logic::GhostColor::CYAN:
        view->setColor(sf::Color::Cyan);
        view->setSpriteType(SpriteGhostType::CYAN);
        break;
    case logic::GhostColor::ORANGE:
        view->setColor(sf::Color(255, 165, 0));
        view->setSpriteType(SpriteGhostType::ORANGE);
        break;
    }

    modelPtr->attach(view);

    if (soundObserver) {
        modelPtr->attach(soundObserver);
    }

    views.push_back(view);
    return model;
}

std::unique_ptr<logic::Wall> ConcreteFactory::createWall(float x, float y, float w, float h) {
    auto model = std::make_unique<logic::Wall>(x, y, w, h);
    logic::Wall* modelPtr = model.get();

    auto view = std::make_shared<WallView>(modelPtr, window, camera, spriteAtlas);
    modelPtr->attach(view);

    views.push_back(view);
    return model;
}

std::unique_ptr<logic::Coin> ConcreteFactory::createCoin(float x, float y) {
    auto model = std::make_unique<logic::Coin>(x, y);
    logic::Coin* modelPtr = model.get();

    auto view = std::make_shared<CoinView>(modelPtr, window, camera, spriteAtlas);
    modelPtr->attach(view);

    if (soundObserver) {
        modelPtr->attach(soundObserver);
    }

    views.push_back(view);
    return model;
}

std::unique_ptr<logic::Fruit> ConcreteFactory::createFruit(float x, float y) {
    auto model = std::make_unique<logic::Fruit>(x, y);
    logic::Fruit* modelPtr = model.get();

    auto view = std::make_shared<FruitView>(modelPtr, window, camera, spriteAtlas);
    modelPtr->attach(view);

    if (soundObserver) {
        modelPtr->attach(soundObserver);
    }

    views.push_back(view);
    return model;
}

void ConcreteFactory::drawAll() {
    for (const auto& view : views) {
        view->draw();
    }
}

void ConcreteFactory::removeDeadViews() {
    views.erase(std::remove_if(views.begin(), views.end(),
                               [](const std::shared_ptr<EntityView>& view) { return view->getModel()->isDead(); }),
                views.end());
}

void ConcreteFactory::updateAll() {
    logic::Stopwatch& stopwatch = logic::Stopwatch::getInstance();
    float deltaTime = stopwatch.getDeltaTime();

    for (const auto& view : views) {
        view->updateAnimation(deltaTime);
    }
}

} // namespace representation