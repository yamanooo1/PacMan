//
// Created by yamanooo on 11/22/25.
//

#include "representation/Game.h"
#include "logic/World.h"
#include "logic/PacMan.h"
#include "logic/Score.h"
#include "logic/Stopwatch.h"
#include "representation/ConcreteFactory.h"
#include "representation/Camera.h"
#include <iostream>

Game::Game()
    : window(nullptr)
    , camera(nullptr)
    , factory(nullptr)
    , world(nullptr)
    , score(nullptr)
    , isRunning(false)
{
}

Game::~Game() {
    // CRITICAL: Destruction order matters!
    // Factory must be destroyed BEFORE World to avoid use-after-free
    // unique_ptr destroys in reverse order of declaration, so we're safe:
    // 1. score (last declared, destroyed first)
    // 2. world
    // 3. factory (destroyed BEFORE world, this is correct!)
    // 4. camera
    // 5. window (first declared, destroyed last)
}

bool Game::initialize(const std::string& mapFile) {
    std::cout << "Initializing game..." << std::endl;

    // Create window
    window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(800, 600),
        "PacMan"
    );

    // Create camera
    camera = std::make_unique<Camera>(800, 600, 10, 10);

    // Create factory (needs window and camera)
    factory = std::make_unique<ConcreteFactory>(window.get(), camera.get());

    // Create world (needs factory)
    world = std::make_unique<World>(factory.get());

    // Create score
    score = std::make_unique<Score>();

    // Connect score to world
    world->setScore(score.get());

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
    std::cout << "Use ARROW KEYS or WASD to move PacMan!" << std::endl;

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
        // Get delta time
        stopwatch.tick();
        float deltaTime = stopwatch.getDeltaTime();

        // Game loop phases
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
            camera->setWindowSize(event.size.width, event.size.height);
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window->setView(sf::View(visibleArea));
        }
    }
}

void Game::handleInput(float deltaTime) {
    PacMan* pacman = world->getPacMan();
    if (!pacman) return;

    // Check keyboard input and set desired direction
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

    // ESC to quit
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window->close();
        isRunning = false;
    }
}

void Game::update(float deltaTime) {
    // Update score decay (uses Stopwatch internally, no deltaTime needed)
    score->updateScoreDecay();

    // Update world (PacMan movement, future: ghost AI, etc.)
    world->update(deltaTime);

    // Optional: Display score in console (comment out to reduce noise)
    // std::cout << "\rScore: " << score->getScore() << std::flush;
}

void Game::render() {
    window->clear(sf::Color::Black);
    factory->drawAll();
    window->display();
}