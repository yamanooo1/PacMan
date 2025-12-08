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

  SoundManager& soundManager = SoundManager::getInstance();
  if (!soundManager.isLoaded()) {
    soundManager.loadSounds("../../resources/sound effects");
  }

  soundManager.stopMusic();
  soundManager.playBackgroundMusic(false);

  if (!loadLevel()) {
    std::cerr << "[LevelState] Failed to load level!" << std::endl;
  }
}

void LevelState::onExit() {
  std::cout << "[LevelState] Exiting level " << currentLevel << std::endl;

  SoundManager& soundManager = SoundManager::getInstance();
  soundManager.stopMusic();
}

// ✅ NEW: Handle window resize by updating camera
void LevelState::onWindowResize(float width, float height) {
  if (camera) {
    // Calculate game area height (total height - HUD height)
    float hudHeight = 60.0f;
    float gameHeight = height - hudHeight;

    camera->setWindowSize(width, gameHeight);
    std::cout << "[LevelState] Camera updated for resize: " << width << "x" << gameHeight << std::endl;
  }
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

  SoundManager& soundManager = SoundManager::getInstance();

  // Track game state changes to reset coin timer
  static bool wasInSpecialState = false;
  bool isInSpecialState = (world->isDeathAnimationActive() ||
                           world->isReadyStateActive() ||
                           world->isLevelClearedDisplayActive());

  // Monitor fear mode state and stop music when it ends naturally
  static bool wasFearModeActive = false;
  bool isFearModeActive = world->isFearModeActive();

  if (wasFearModeActive && !isFearModeActive) {
    soundManager.stopFearModeSound();
    std::cout << "[LevelState] Fear mode ended naturally - stopped music" << std::endl;
  }

  wasFearModeActive = isFearModeActive;

  // Count coins BEFORE update
  static int previousCoinCount = -1;
  int coinsBeforeUpdate = 0;

  if (world) {
    for (const auto& entity : world->getEntities()) {
      if (entity->isDead()) continue;
      float w = entity->getWidth();
      float h = entity->getHeight();
      bool isCoin = (w > 0.015f && w < 0.025f && h > 0.015f && h < 0.025f);
      if (isCoin) coinsBeforeUpdate++;
    }
  }

  if (previousCoinCount == -1) {
    previousCoinCount = coinsBeforeUpdate;
  }

  // Update world
  world->update(deltaTime);

  // Count coins AFTER update
  int coinsAfterUpdate = 0;
  for (const auto& entity : world->getEntities()) {
    if (entity->isDead()) continue;
    float w = entity->getWidth();
    float h = entity->getHeight();
    bool isCoin = (w > 0.015f && w < 0.025f && h > 0.015f && h < 0.025f);
    if (isCoin) coinsAfterUpdate++;
  }

  // Detect coin collection THIS frame
  bool coinWasCollectedThisFrame = (coinsAfterUpdate < previousCoinCount);
  previousCoinCount = coinsAfterUpdate;

  // Freeze during special states
  if (world->isLevelClearedDisplayActive()) {
    return;
  }

  // Handle input
  handleInput();

  PacMan* pacman = world->getPacMan();

  // Only play movement sound during normal gameplay
  bool isNormalGameplay = (pacman && lives && !lives->isGameOver() &&
                           world && !world->isDeathAnimationActive() &&
                           !world->isReadyStateActive() &&
                           !world->isLevelClearedDisplayActive());

  if (isNormalGameplay) {
    // Detect state transition
    bool justExitedSpecialState = (wasInSpecialState && !isInSpecialState);

    // Track position for movement detection
    static float prevX = 0.0f;
    static float prevY = 0.0f;
    auto [currentX, currentY] = pacman->getPosition();

    bool actuallyMoving = (std::abs(currentX - prevX) > 0.001f ||
                           std::abs(currentY - prevY) > 0.001f);

    Direction currentDirection = pacman->getDirection();
    bool wantsToMove = (currentDirection != Direction::NONE);

    // Coin eating sound: Track recent coin collection with timer
    static float timeSinceLastCoin = 999.0f;

    // Reset timer when exiting special states
    if (justExitedSpecialState) {
      timeSinceLastCoin = 999.0f;
      std::cout << "[SOUND] Reset coin timer on state transition" << std::endl;
    } else if (coinWasCollectedThisFrame) {
      timeSinceLastCoin = 0.0f;
    } else {
      timeSinceLastCoin += deltaTime;
    }

    // Play sound only if moving through coins recently
    if (actuallyMoving && wantsToMove && timeSinceLastCoin < 0.3f && coinsAfterUpdate > 0) {
      soundManager.startMovementSound();
    } else {
      soundManager.stopMovementSound();
    }

    // Reset timer when stopped
    if (!actuallyMoving) {
      timeSinceLastCoin = 999.0f;
    }

    prevX = currentX;
    prevY = currentY;
  } else {
    soundManager.stopMovementSound();
  }

  wasInSpecialState = isInSpecialState;

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