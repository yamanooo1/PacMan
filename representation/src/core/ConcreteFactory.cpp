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

/**
 * @brief Initialize factory with rendering context
 *
 * Creates empty sprite atlas (populated via loadSprites).
 */
ConcreteFactory::ConcreteFactory(sf::RenderWindow& win, std::shared_ptr<Camera> cam)
    : window(win), camera(std::move(cam)), spriteAtlas(std::make_shared<SpriteAtlas>()), soundObserver(nullptr) {}

ConcreteFactory::~ConcreteFactory() = default;

/**
 * @brief Load sprite sheet texture
 *
 * Must succeed before entity creation (views need texture).
 */
bool ConcreteFactory::loadSprites(const std::string& filepath) {
    if (!spriteAtlas) {
        return false;
    }

    return spriteAtlas->loadFromFile(filepath);
}

/**
 * @brief Create PacMan model+view pair
 *
 * Observer chain:
 * - PacManView: attached (for animations)
 * - SoundObserver: attached (for movement sounds)
 * - Lives: attached by World (not here)
 *
 * @return Unique ptr to PacMan model (ownership to World)
 */
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

/**
 * @brief Create Ghost model+view pair with color configuration
 *
 * View setup:
 * - Set fill color for fallback rendering (if sprites fail)
 * - Set sprite type for correct sprite atlas lookup
 *
 * Observer chain:
 * - GhostView: attached (for fear mode animation)
 * - SoundObserver: attached (for eaten sound)
 * - Score: attached by World (for points on eaten)
 */
std::unique_ptr<logic::Ghost> ConcreteFactory::createGhost(float x, float y, logic::GhostType type,
                                                           logic::GhostColor color, float waitTime,
                                                           float speedMultiplier) {
    auto model = std::make_unique<logic::Ghost>(x, y, type, color, waitTime, speedMultiplier);
    logic::Ghost* modelPtr = model.get();

    auto view = std::make_shared<GhostView>(modelPtr, window, camera, spriteAtlas);

    // Configure view color based on ghost color
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

/**
 * @brief Create Wall model+view pair
 *
 * Walls are static (no animation, no sound).
 */
std::unique_ptr<logic::Wall> ConcreteFactory::createWall(float x, float y, float w, float h) {
    auto model = std::make_unique<logic::Wall>(x, y, w, h);
    logic::Wall* modelPtr = model.get();

    auto view = std::make_shared<WallView>(modelPtr, window, camera, spriteAtlas);
    modelPtr->attach(view);

    views.push_back(view);
    return model;
}

/**
 * @brief Create Coin model+view pair
 *
 * Observer chain:
 * - CoinView: attached
 * - SoundObserver: attached (for collection sound)
 * - Score: attached by World (for points)
 */
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

/**
 * @brief Create Fruit model+view pair
 *
 * Observer chain:
 * - FruitView: attached
 * - SoundObserver: attached (for power-up sound + fear mode music)
 * - Score: attached by World (for points)
 */
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

/**
 * @brief Render all views (called once per frame)
 *
 * Views handle their own visibility checks (isDead, etc.).
 */
void ConcreteFactory::drawAll() {
    for (const auto& view : views) {
        view->draw();
    }
}

/**
 * @brief Remove views for dead models
 *
 * Two-phase cleanup (called before World erases models):
 * 1. Factory removes views (this method)
 * 2. World removes models
 *
 * Prevents views from holding dangling model pointers.
 */
void ConcreteFactory::removeDeadViews() {
    views.erase(std::remove_if(views.begin(), views.end(),
                               [](const std::shared_ptr<EntityView>& view) { return view->getModel()->isDead(); }),
                views.end());
}

/**
 * @brief Update view animations (called once per frame)
 *
 * Uses Stopwatch singleton for consistent deltaTime across all views.
 * Views update sprite frame timers for smooth animation.
 */
void ConcreteFactory::updateAll() {
    logic::Stopwatch& stopwatch = logic::Stopwatch::getInstance();
    float deltaTime = stopwatch.getDeltaTime();

    for (const auto& view : views) {
        view->updateAnimation(deltaTime);
    }
}

} // namespace representation