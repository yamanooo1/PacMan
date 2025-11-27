
#include <iostream>
#include <SFML/Graphics.hpp>
#include "logic/World.h"
#include "logic/PacMan.h"
#include "logic/Stopwatch.h"
#include "representation/ConcreteFactory.h"
#include "representation/Camera.h"
#include "logic/Score.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "PacMan Test");

    Camera camera(800, 600, 10, 10);
    ConcreteFactory factory(&window, &camera);
    World world(&factory);
    Score score;
    world.setScore(&score);

    if (!world.loadFromFile("../../logic/include/map/map1.txt")) {
        std::cerr << "Failed to load map!" << std::endl;
        return 1;
    }

    camera.setMapSize(world.getMapWidth(), world.getMapHeight());

    std::cout << "Map loaded: " << world.getMapWidth() << "x" << world.getMapHeight() << std::endl;
    std::cout << "Entities: " << world.getEntityCount() << std::endl;
    std::cout << "Use ARROW KEYS or WASD to move PacMan!" << std::endl;

    Stopwatch& stopwatch = Stopwatch::getInstance();

    while (window.isOpen()) {
        stopwatch.tick();
        float deltaTime = stopwatch.getDeltaTime();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized) {
                camera.setWindowSize(event.size.width, event.size.height);
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }

        // Input handling - set direction based on keys
      PacMan* pacman = world.getPacMan();
      if (pacman) {
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

        world.update(deltaTime);
        std::cout << "\rScore: " << score.getScore() << std::flush;

        window.clear(sf::Color::Black);
        factory.drawAll();
        window.display();
    }

    return 0;
}