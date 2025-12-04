#include "representation/LevelState.h"
#include "representation/StateManager.h"
#include "representation/PausedState.h"
#include "representation/VictoryState.h"
#include "representation/ConcreteFactory.h"
#include "representation/Camera.h"
#include "representation/HUD.h"
#include "logic/World.h"
#include "logic/Score.h"
#include "logic/Lives.h"
#include "logic/PacMan.h"
#include <iostream>


LevelState::~LevelState() = default;

LevelState::LevelState(int level)
    : currentLevel(level)
    , mapFile("../../resources/map/map1.txt")
    , pauseRequested(false)
{
}

void LevelState::onEnter() {
  std::cout << "[LevelState] Entering level " << currentLevel << std::endl;

  if (!loadLevel()) {
    std::cerr << "[LevelState] Failed to load level!" << std::endl;
    // TODO: Handle error - maybe go back to menu?
  }
}

void LevelState::onExit() {
  std::cout << "[LevelState] Exiting level " << currentLevel << std::endl;
}

bool LevelState::loadLevel() {
  std::cout << "[LevelState] Loading level " << currentLevel << "..." << std::endl;

  // Create camera (game area: 800x800, HUD: 60px)
  int gameHeight = 800;
  int hudHeight = 60;
  camera = std::make_unique<Camera>(800, gameHeight, 10, 10);

  // Create factory (we'll get the window from render())
  // For now, we can't create factory here because we don't have window yet
  // We'll do this in render() on first frame

  // Create HUD
  hud = std::make_unique<HUD>(nullptr, hudHeight);  // Set window later
  hud->loadFont("../../resources/fonts/font-emulogic/emulogic.ttf");

  // Create score and lives
  score = std::make_unique<Score>();
  lives = std::make_unique<Lives>(3);

  std::cout << "[LevelState] Level " << currentLevel << " loaded successfully!" << std::endl;
  return true;
}

void LevelState::handleEvents(sf::RenderWindow& window) {
  // ✅ IMPROVED: Only trigger pause on key press, not while held
  static bool escWasPressed = false;
  bool escIsPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);

  // Detect ESC key press (not held)
  if (escIsPressed && !escWasPressed) {
    std::cout << "[LevelState] Pause requested" << std::endl;
    pauseRequested = true;

    // Push PausedState onto stack
    if (stateManager) {
      stateManager->pushState(std::make_unique<PausedState>());
    }
  }

  escWasPressed = escIsPressed;
}

void LevelState::update(float deltaTime) {
  // Don't update if pause was just requested
  if (pauseRequested) {
    pauseRequested = false;
    return;
  }

  if (!world) {
    return;
  }

  if (world->isLevelCleared()) {
    return;
  }

  // Handle input (PacMan movement)
  handleInput();

  // ✅ IMPROVED: Only update score when game is actually playable
  // Not during: death animation, ready state, or game over
  if (score && lives && !lives->isGameOver() &&
      world && !world->isDeathAnimationActive() && !world->isReadyStateActive()) {
    score->updateScoreDecay();
      }

  // Update world
  world->update(deltaTime);

  // Check for game over FIRST
  if (lives && lives->isGameOver()) {
    std::cout << "[LevelState] Game Over! Final score: " << score->getScore() << std::endl;

    if (stateManager) {
      stateManager->pushState(std::make_unique<VictoryState>(0, score->getScore()));
    }
    return;
  }

  // ✅ Check for level complete (only push VictoryState once)
  if (world->isLevelCleared() && !world->isLevelClearedDisplayActive()) {
    // Check if VictoryState is already on top
    if (stateManager && stateManager->getCurrentState() == this) {
      std::cout << "[LevelState] Level complete! Score: " << score->getScore() << std::endl;
      stateManager->pushState(std::make_unique<VictoryState>(currentLevel + 1, score->getScore()));
    }
  }
}

void LevelState::render(sf::RenderWindow& window) {
  // Lazy initialization of factory and world (need window reference)
  if (!factory) {
    factory = std::make_unique<ConcreteFactory>(&window, camera.get());

    // Load sprites
    if (!factory->loadSprites("../../resources/sprites/spritemap.png")) {
      std::cerr << "[LevelState] Failed to load sprites!" << std::endl;
      return;
    }

    // Update HUD window reference
    hud = std::make_unique<HUD>(&window, 60);
    hud->loadFont("../../resources/fonts/font-emulogic/emulogic.ttf");

    // Create world
    world = std::make_unique<World>(factory.get(), currentLevel);
    world->setScore(score.get());
    world->setLives(lives.get());

    // Load map
    if (!world->loadFromFile(mapFile)) {
      std::cerr << "[LevelState] Failed to load map: " << mapFile << std::endl;
      return;
    }

    // Update camera with map dimensions
    camera->setMapSize(world->getMapWidth(), world->getMapHeight());

    std::cout << "[LevelState] Map loaded: " << world->getMapWidth() << "x"
              << world->getMapHeight() << std::endl;
    std::cout << "[LevelState] Entities: " << world->getEntityCount() << std::endl;

    // Start with ready state
    world->startReadyState();
  }

  if (!world) return;

  // Update animations and draw
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