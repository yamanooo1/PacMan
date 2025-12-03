#ifndef HUD_H
#define HUD_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class World;
class Score;
class Lives;

class HUD {
public:
  HUD(sf::RenderWindow* window, int hudHeight);

  void loadFont(const std::string& fontPath);
  void draw(World* world, Score* score, Lives* lives);
  void drawReadyText(World* world);  // ✅ ADD THIS

private:
  sf::RenderWindow* window;
  sf::Font font;
  int hudHeight;
  bool fontLoaded;
};

#endif // HUD_H