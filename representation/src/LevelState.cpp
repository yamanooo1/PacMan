#include "representation/LevelState.h"
#include "representation/StateManager.h"
#include "representation/PausedState.h"
#include "representation/VictoryState.h"
#include "representation/ConcreteFactory.h"
#include "representation/SoundManager.h"
#include "representation/SoundObserver.h"
#include "representation/Camera.h"
#include "representation/HUD.h"
#include "logic/World.h"
#include "logic/Score.h"
#include "logic/Lives.h"
#include "logic/PacMan.h"
#include <iostream>


LevelState::~LevelState() = default;

LevelState::LevelState(int level, int startingScore)
    : currentLevel(level)
    , initialScore(startingScore)
    , mapFile("../../resources/map/map1.txt")
    , pauseRequested(false)
{
  std::cout << "[LevelState] Constructor - Level: " << level
            << ", Starting Score: " << startingScore << std::endl;
}

void LevelState::onEnter() {
  std::cout << "[LevelState] Entering level " << currentLevel << std::endl;

  // ✅ Load sounds FIRST
  SoundManager& soundManager = SoundManager::getInstance();
  if (!soundManager.isLoaded()) {
    soundManager.loadSounds("../../resources/sound effects");
  }

  // ✅ Play intro music ONCE (don't loop)
  soundManager.stopMusic();
  soundManager.playBackgroundMusic(false);  // false = play once

  if (!loadLevel()) {
    std::cerr << "[LevelState] Failed to load level!" << std::endl;
  }
}

void LevelState::onExit() {
  std::cout << "[LevelState] Exiting level " << currentLevel << std::endl;

  // ✅ Stop all sounds
  SoundManager& soundManager = SoundManager::getInstance();
  soundManager.stopMusic();
}

bool LevelState::loadLevel() {
  std::cout << "[LevelState] Loading level " << currentLevel << "..." << std::endl;

  int gameHeight = 800;
  int hudHeight = 60;
  camera = std::make_shared<Camera>(800, gameHeight, 10, 10);
  std::cout << "[LevelState] ✅ Camera created" << std::endl;

  std::cout << "[LevelState] Creating Score with initialScore: " << initialScore << std::endl;
  score = std::make_shared<Score>();
  score->setScore(initialScore);
  std::cout << "[LevelState] ✅ Score set to: " << initialScore << std::endl;

  lives = std::make_shared<Lives>(3);
  std::cout << "[LevelState] ✅ Lives created" << std::endl;

  std::cout << "[LevelState] ✅ Sound system ready" << std::endl;
  std::cout << "[LevelState] Level " << currentLevel << " loaded successfully!" << std::endl;
  return true;
}

void LevelState::handleEvents(sf::RenderWindow& window) {
  if (world && world->isLevelCleared()) {
    return;
  }

  static bool escWasPressed = false;
  bool escIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);

  if (escIsPressed && !escWasPressed) {
    std::cout << "[LevelState] Pause requested" << std::endl;
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

  static int frameCount = 0;
  frameCount++;
  if (frameCount % 60 == 0) {
    std::cout << "[DEBUG LevelState] Frame " << frameCount
              << " | levelCleared=" << world->isLevelCleared()
              << " | displayActive=" << world->isLevelClearedDisplayActive()
              << " | readyActive=" << world->isReadyStateActive()
              << " | deathActive=" << world->isDeathAnimationActive()
              << std::endl;
  }

  // Update world
  world->update(deltaTime);

  // Freeze during special states
  if (world->isLevelClearedDisplayActive()) {
    std::cout << "[DEBUG] Skipping handleInput - level cleared display active!" << std::endl;
    return;
  }

  // Handle input
  handleInput();

  // ✅ PERFECT SOLUTION: Check BOTH direction AND actual movement
  SoundManager& soundManager = SoundManager::getInstance();
  PacMan* pacman = world->getPacMan();

  // Only play movement sound during normal gameplay
  bool isNormalGameplay = (pacman && lives && !lives->isGameOver() &&
                           world && !world->isDeathAnimationActive() &&
                           !world->isReadyStateActive() &&
                           !world->isLevelClearedDisplayActive());

  if (isNormalGameplay) {
    // ✅ Track position to detect ACTUAL movement
    static float prevX = 0.0f;
    static float prevY = 0.0f;
    auto [currentX, currentY] = pacman->getPosition();

    bool actuallyMoving = (std::abs(currentX - prevX) > 0.001f ||
                           std::abs(currentY - prevY) > 0.001f);

    // ✅ Also check direction isn't NONE (not stopped by user)
    Direction currentDirection = pacman->getDirection();
    bool wantsToMove = (currentDirection != Direction::NONE);

    // ✅ Play sound only if BOTH:
    // 1. PacMan wants to move (direction set)
    // 2. PacMan is ACTUALLY moving (position changing)
    if (actuallyMoving && wantsToMove) {
      soundManager.startMovementSound();
    } else {
      soundManager.stopMovementSound();
    }

    prevX = currentX;
    prevY = currentY;
  } else {
    // ✅ Not normal gameplay - stop sound
    soundManager.stopMovementSound();
  }

  // Update score decay
  if (score && lives && !lives->isGameOver() &&
      world && !world->isDeathAnimationActive() &&
      !world->isReadyStateActive() &&
      !world->isLevelClearedDisplayActive()) {
    score->updateScoreDecay();
  }

  // Check for game over
  if (lives && lives->isGameOver()) {
    std::cout << "[LevelState] Game Over! Final score: " << score->getScore() << std::endl;

    if (stateManager) {
      stateManager->pushState(std::make_unique<VictoryState>(0, score->getScore()));
    }
    return;
  }

  // Check for level complete
  if (world->isLevelCleared()) {
    std::cout << "[DEBUG] Level cleared: true, Display active: "
              << (world->isLevelClearedDisplayActive() ? "true" : "false") << std::endl;
  }

  if (world->isLevelCleared() && !world->isLevelClearedDisplayActive()) {
    std::cout << "[LevelState] ✅ PUSHING VICTORYSTATE now!" << std::endl;
    stateManager->pushState(std::make_unique<VictoryState>(currentLevel + 1, score->getScore()));
  }
}

void LevelState::render(sf::RenderWindow& window) {
  if (!factory) {
    factory = std::make_unique<ConcreteFactory>(window, camera);

    if (!factory->loadSprites("../../resources/sprites/spritemap.png")) {
      std::cerr << "[LevelState] Failed to load sprites!" << std::endl;
      return;
    }

    auto soundObserver = std::make_shared<SoundObserver>();
    factory->setSoundObserver(soundObserver);
    std::cout << "[LevelState] ✅ SoundObserver attached to factory" << std::endl;

    hud = std::make_unique<HUD>(window, 60);
    hud->loadFont("../../resources/fonts/font-emulogic/emulogic.ttf");

    world = std::make_unique<World>(factory.get(), currentLevel);
    world->setScore(score);
    world->setLives(lives);

    if (!world->loadFromFile(mapFile)) {
      std::cerr << "[LevelState] Failed to load map: " << mapFile << std::endl;
      return;
    }

    camera->setMapSize(world->getMapWidth(), world->getMapHeight());

    std::cout << "[LevelState] Map loaded: " << world->getMapWidth() << "x"
              << world->getMapHeight() << std::endl;
    std::cout << "[LevelState] Entities: " << world->getEntityCount() << std::endl;

    world->startReadyState();
  }

  if (!world) return;

  factory->updateAll();
  factory->drawAll();
  hud->draw(world.get(), score.get(), lives.get(), currentLevel);
  hud->drawReadyText(world.get());
  hud->drawLevelClearedText(world.get());
}

void LevelState::handleInput() {
  if (!world) return;

  PacMan* pacman = world->getPacMan();
  if (!pacman) return;

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
      sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
    pacman->setDesiredDirection(Direction::LEFT);
  }
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
    pacman->setDesiredDirection(Direction::RIGHT);
  }
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
    pacman->setDesiredDirection(Direction::UP);
  }
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
    pacman->setDesiredDirection(Direction::DOWN);
  }
}