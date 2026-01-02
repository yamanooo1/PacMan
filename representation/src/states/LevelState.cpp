#include "../../include/representation/states/LevelState.h"
#include "../../../logic/include/logic/core/World.h"
#include "../../../logic/include/logic/entities/PacMan.h"
#include "../../../logic/include/logic/systems/Lives.h"
#include "../../../logic/include/logic/systems/Score.h"
#include "../../include/representation/core/Camera.h"
#include "../../include/representation/core/ConcreteFactory.h"
#include "../../include/representation/states/PausedState.h"
#include "../../include/representation/states/StateManager.h"
#include "../../include/representation/states/VictoryState.h"
#include "../../include/representation/systems/HUD.h"
#include "../../include/representation/systems/SoundManager.h"
#include "../../include/representation/systems/SoundObserver.h"
#include <cmath>

/**
 * LevelState.cpp - Active Gameplay Implementation
 *
 * KEY METHODS:
 *
 * LevelState():
 *   - Store level and score
 *   - Set map file path
 *
 * loadLevel():
 *   - Create camera (game area = window height - 60px)
 *   - Create score system (with carried score)
 *   - Create lives system (3 lives)
 *
 * onEnter():
 *   - Load sounds (if first time)
 *   - Stop music
 *   - Play background music (level 1 only)
 *
 * First render():
 *   - Create factory (needs window reference)
 *   - Load sprite sheet
 *   - Attach sound observer
 *   - Create HUD
 *   - Create world (pass factory)
 *   - Attach score/lives to world
 *   - Load map (creates entities)
 *   - Start READY! state
 *
 * handleInput():
 *   - Map keys → setDesiredDirection()
 *
 * update(deltaTime):
 *   - Count coins before update
 *   - world->update(deltaTime)
 *   - Count coins after update
 *   - Manage movement sound based on:
 *     * Movement detection
 *     * Coin collection timing
 *     * Special state flags
 *   - Handle state transitions:
 *     * Game over → VictoryState(0, score)
 *     * Level cleared → VictoryState(nextLevel, score)
 *
 * render():
 *   - Lazy init (first call only)
 *   - factory->updateAll() (animations)
 *   - factory->drawAll() (entities)
 *   - hud->draw() (UI)
 *   - hud->drawReadyText() (if ready state)
 *   - hud->drawLevelClearedText() (if cleared)
 *
 * SOUND MANAGEMENT COMPLEXITY:
 *
 * Movement sound plays when:
 *   actuallyMoving (position delta > 0.001)
 *   AND wantsToMove (direction != NONE)
 *   AND timeSinceLastCoin < 0.3s
 *   AND coinsRemaining > 0
 *   AND isNormalGameplay (not death/ready/cleared)
 *
 * Special state handling resets timeSinceLastCoin.
 *
 * The original .cpp file (424 lines) is preserved as-is.
 * This file documents the architecture and key logic flows.
 */

namespace representation {

LevelState::~LevelState() = default;

LevelState::LevelState(int level, int startingScore)
    : currentLevel(level), initialScore(startingScore), mapFile("../../resources/map/map1.txt"), pauseRequested(false),
      windowWidth(800.0f), windowHeight(860.0f) {}

void LevelState::onEnter() {
    SoundManager& soundManager = SoundManager::getInstance();
    if (!soundManager.isLoaded()) {
        soundManager.loadSounds("../../resources/Sounds");
    }

    soundManager.stopMusic();

    if (currentLevel == 1) {
        soundManager.playBackgroundMusic(false);
    }

    loadLevel();
}

void LevelState::onExit() {
    SoundManager& soundManager = SoundManager::getInstance();
    soundManager.stopMusic();
}

void LevelState::onWindowResize(float width, float height) {
    windowWidth = width;
    windowHeight = height;

    if (camera) {
        float gameHeight = height - HUD_HEIGHT;
        camera->setWindowSize(width, gameHeight);
    }
}

bool LevelState::loadLevel() {
    float gameHeight = windowHeight - HUD_HEIGHT;
    camera = std::make_shared<Camera>(windowWidth, gameHeight, 10, 10);

    score = std::make_shared<logic::Score>();
    score->setScore(initialScore);

    lives = std::make_shared<logic::Lives>(3);

    return true;
}

void LevelState::handleEvents(sf::RenderWindow& window) {
    static bool escWasPressed = false;
    bool escIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);

    // Blokkeer input tijdens speciale states
    if (world && (world->isDeathAnimationActive() || world->isLevelCleared())) {
        // CONSUMEER de ESC key - markeer als "al verwerkt"
        escWasPressed = true; // Forceer naar true om trigger te voorkomen
        return;
    }

    // Normale pause handling
    if (escIsPressed && !escWasPressed) {
        pauseRequested = true;

        if (stateManager) {
            stateManager->pushState(std::make_unique<PausedState>());
        }
    }

    escWasPressed = escIsPressed;
}

void LevelState::update(float deltaTime) {
    if (pauseRequested) {
        pauseRequested = false;
        return;
    }

    if (!world) {
        return;
    }

    SoundManager& soundManager = SoundManager::getInstance();

    static bool wasInSpecialState = false;
    bool isInSpecialState =
        (world->isDeathAnimationActive() || world->isReadyStateActive() || world->isLevelClearedDisplayActive());

    static bool wasFearModeActive = false;
    bool isFearModeActive = world->isFearModeActive();

    if (wasFearModeActive && !isFearModeActive) {
        soundManager.stopFearModeSound();
    }

    wasFearModeActive = isFearModeActive;

    static int previousCoinCount = -1;
    int coinsBeforeUpdate = 0;

    if (world) {
        for (const auto& entity : world->getEntities()) {
            if (entity->isDead())
                continue;
            float w = entity->getWidth();
            float h = entity->getHeight();
            bool isCoin = (w > 0.015f && w < 0.025f && h > 0.015f && h < 0.025f);
            if (isCoin)
                coinsBeforeUpdate++;
        }
    }

    if (previousCoinCount == -1) {
        previousCoinCount = coinsBeforeUpdate;
    }

    world->update(deltaTime);

    int coinsAfterUpdate = 0;
    for (const auto& entity : world->getEntities()) {
        if (entity->isDead())
            continue;
        float w = entity->getWidth();
        float h = entity->getHeight();
        bool isCoin = (w > 0.015f && w < 0.025f && h > 0.015f && h < 0.025f);
        if (isCoin)
            coinsAfterUpdate++;
    }

    bool coinWasCollectedThisFrame = (coinsAfterUpdate < previousCoinCount);
    previousCoinCount = coinsAfterUpdate;

    if (world->isLevelClearedDisplayActive()) {
        soundManager.stopMovementSound();
        soundManager.stopFearModeSound();
        return;
    }

    handleInput();

    logic::PacMan* pacman = world->getPacMan();

    bool isNormalGameplay = (pacman && lives && !lives->isGameOver() && world && !world->isDeathAnimationActive() &&
                             !world->isReadyStateActive() && !world->isLevelClearedDisplayActive());

    if (isNormalGameplay) {
        bool justExitedSpecialState = (wasInSpecialState && !isInSpecialState);

        static float prevX = 0.0f;
        static float prevY = 0.0f;
        auto [currentX, currentY] = pacman->getPosition();

        bool actuallyMoving = (std::abs(currentX - prevX) > 0.001f || std::abs(currentY - prevY) > 0.001f);

        logic::Direction currentDirection = pacman->getDirection();
        bool wantsToMove = (currentDirection != logic::Direction::NONE);

        static float timeSinceLastCoin = 999.0f;

        if (justExitedSpecialState) {
            timeSinceLastCoin = 999.0f;
        } else if (coinWasCollectedThisFrame) {
            timeSinceLastCoin = 0.0f;
        } else {
            timeSinceLastCoin += deltaTime;
        }

        if (actuallyMoving && wantsToMove && timeSinceLastCoin < 0.3f && coinsAfterUpdate > 0) {
            soundManager.startMovementSound();
        } else {
            soundManager.stopMovementSound();
        }

        if (!actuallyMoving) {
            timeSinceLastCoin = 999.0f;
        }

        prevX = currentX;
        prevY = currentY;
    } else {
        soundManager.stopMovementSound();
    }

    wasInSpecialState = isInSpecialState;

    if (score && lives && !lives->isGameOver() && world && !world->isDeathAnimationActive() &&
        !world->isReadyStateActive() && !world->isLevelClearedDisplayActive()) {
        score->updateScoreDecay();
    }

    if (lives && lives->isGameOver()) {
        if (!world->isDeathAnimationActive()) {
            if (stateManager) {
                stateManager->pushState(std::make_unique<VictoryState>(0, score->getScore()));
            }
            return;
        }
        return;
    }

    if (world->isLevelCleared() && !world->isLevelClearedDisplayActive()) {
        stateManager->clearAndPushState(std::make_unique<LevelState>(currentLevel + 1, score->getScore()));
    }
}

void LevelState::render(sf::RenderWindow& window) {
    if (!factory) {
        factory = std::make_unique<ConcreteFactory>(window, camera);

        if (!factory->loadSprites("../../resources/sprites/spritemap.png")) {
            return;
        }

        auto soundObserver = std::make_shared<SoundObserver>();
        factory->setSoundObserver(soundObserver);

        hud = std::make_unique<HUD>(window, static_cast<int>(HUD_HEIGHT));
        hud->loadFont("../../resources/fonts/font-emulogic/emulogic.ttf");

        world = std::make_unique<logic::World>(factory.get(), currentLevel);
        world->setScore(score);
        world->setLives(lives);

        if (!world->loadFromFile(mapFile)) {
            return;
        }

        camera->setMapSize(world->getMapWidth(), world->getMapHeight());

        world->startReadyState();
    }

    if (!world)
        return;

    factory->updateAll();
    factory->drawAll();
    hud->draw(world.get(), score.get(), lives.get(), currentLevel);
    hud->drawReadyText(world.get());
    hud->drawLevelClearedText(world.get());
}

void LevelState::handleInput() {
    if (!world)
        return;

    logic::PacMan* pacman = world->getPacMan();
    if (!pacman)
        return;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        pacman->setDesiredDirection(logic::Direction::LEFT);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        pacman->setDesiredDirection(logic::Direction::RIGHT);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        pacman->setDesiredDirection(logic::Direction::UP);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        pacman->setDesiredDirection(logic::Direction::DOWN);
    }
}

} // namespace representation