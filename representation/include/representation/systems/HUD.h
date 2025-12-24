#ifndef HUD_H
#define HUD_H

#include <SFML/Graphics.hpp>
#include <string>

namespace logic {
class World;
class Score;
class Lives;
}

namespace representation {

class HUD {
public:
    HUD(sf::RenderWindow& window, int hudHeight);

    void loadFont(const std::string& fontPath);
    void draw(logic::World* world, logic::Score* score, logic::Lives* lives, int currentLevel);
    void drawReadyText(logic::World* world);
    void drawLevelClearedText(logic::World* world);

private:
    sf::RenderWindow& window;
    sf::Font font;
    int hudHeight;
    bool fontLoaded;
};

} // namespace representation

#endif