
#include <iostream>
#include <SFML/Graphics.hpp>
#include "logic/World.h"
#include "representation/ConcreteFactory.h"
#include "representation/Camera.h"

int main() {
  // Create window
  sf::RenderWindow window(sf::VideoMode(800, 600), "PacMan Test");

  // Create factory and world
  // Create camera with initial guess (will update after loading map)
  Camera camera(800, 600, 10, 10);
  ConcreteFactory factory(&window, &camera);
  World world(&factory);

  // Load map
  if (!world.loadFromFile("../../logic/include/map/map1.txt")) {
    std::cerr << "Failed to load map!" << std::endl;
    return 1;
  }

  std::cout << "Map loaded: " << world.getMapWidth() << "x" << world.getMapHeight() << std::endl;
  std::cout << "Entities: " << world.getEntityCount() << std::endl;

  // Game loop
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear(sf::Color::Black);
    factory.drawAll();
    window.display();
  }

  return 0;
}