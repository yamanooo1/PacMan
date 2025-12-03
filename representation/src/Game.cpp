#include "representation/Game.h"
#include "representation/HUD.h"
#include "logic/World.h"
#include "logic/PacMan.h"
#include "logic/Score.h"
#include "logic/Lives.h"
#include "logic/Stopwatch.h"
#include "representation/ConcreteFactory.h"
#include "representation/Camera.h"
#include <iostream>

Game::Game()
    : window(nullptr)
    , camera(nullptr)
    , factory(nullptr)
    , hud(nullptr)
    , world(nullptr)
    , score(nullptr)
    , lives(nullptr)
    , isRunning(false)
{
}

Game::~Game() {
}

bool Game::initialize(const std::string& mapFile) {
    std::cout << "Initializing game..." << std::endl;

    // Create window with extra space for HUD
    int gameHeight = 800;
    int hudHeight = 60;
    window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(800, gameHeight + hudHeight),
        "PacMan"
    );

    // Create camera (game area only, not including HUD)
    camera = std::make_unique<Camera>(800, gameHeight, 10, 10);

    // Create factory
    factory = std::make_unique<ConcreteFactory>(window.get(), camera.get());

    // Load sprites
    if (!factory->loadSprites("../../resources/sprites/spritemap.png")) {
        std::cerr << "Failed to load sprites!" << std::endl;
        return false;
    }

    // Create HUD
    hud = std::make_unique<HUD>(window.get(), hudHeight);
    hud->loadFont("../../resources/fonts/font-emulogic/emulogic.ttf");

    // Create world
    world = std::make_unique<World>(factory.get());

    // Create score and lives
    score = std::make_unique<Score>();
    lives = std::make_unique<Lives>(3);

    // Connect score and lives to world
    world->setScore(score.get());
    world->setLives(lives.get());

    // Load map
    if (!world->loadFromFile(mapFile)) {
        std::cerr << "Failed to load map: " << mapFile << std::endl;
        return false;
    }

    // Update camera with map dimensions
    camera->setMapSize(world->getMapWidth(), world->getMapHeight());

    std::cout << "Map loaded: " << world->getMapWidth() << "x"
              << world->getMapHeight() << std::endl;
    std::cout << "Entities: " << world->getEntityCount() << std::endl;
    std::cout << "Lives: " << lives->getLives() << std::endl;
    std::cout << "Use ARROW KEYS or WASD to move PacMan!" << std::endl;

    world->startReadyState();

    isRunning = true;
    return true;
}

void Game::run() {
    if (!isRunning) {
        std::cerr << "Game not initialized! Call initialize() first." << std::endl;
        return;
    }

    Stopwatch& stopwatch = Stopwatch::getInstance();

    while (window->isOpen() && isRunning) {
        stopwatch.tick();
        float deltaTime = stopwatch.getDeltaTime();

        handleEvents();
        handleInput(deltaTime);
        update(deltaTime);
        render();
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
            isRunning = false;
        }

        if (event.type == sf::Event::Resized) {
            // Update window view to maintain aspect ratio
            float windowWidth = static_cast<float>(event.size.width);
            float windowHeight = static_cast<float>(event.size.height);

            // Set view to full window
            sf::FloatRect visibleArea(0, 0, windowWidth, windowHeight);
            window->setView(sf::View(visibleArea));

            // Update camera size (game area is window height minus HUD)
            int hudHeight = 60;
            camera->setWindowSize(windowWidth, windowHeight - hudHeight);
        }
    }
}

void Game::handleInput(float deltaTime) {
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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window->close();
        isRunning = false;
    }
}

void Game::update(float deltaTime) {
  // Only update score decay if game is active (not dead and not game over)
  if (lives && !lives->isGameOver() && world && !world->isDeathAnimationActive()) {
    score->updateScoreDecay();
  }

  world->update(deltaTime);
}

void Game::render() {
  window->clear(sf::Color::Black);

  if (world) {
    factory->updateAll();
    factory->drawAll();
    hud->draw(world.get(), score.get(), lives.get());
    hud->drawReadyText(world.get());  // ✅ ADD THIS - Draw ready text on top
  }

  window->display();
}
