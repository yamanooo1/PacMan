#ifndef PACMAN_SPRITEATLAS_H
#define PACMAN_SPRITEATLAS_H

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>

enum class Direction;

enum class PacManFrame { CLOSED, HALF_OPEN, FULL_OPEN };

enum class GhostFrame { FRAME_1, FRAME_2 };

enum class SpriteGhostType { RED, PINK, CYAN, ORANGE };

enum class DeathFrame {
    FRAME_0,
    FRAME_1,
    FRAME_2,
    FRAME_3,
    FRAME_4,
    FRAME_5,
    FRAME_6,
    FRAME_7,
    FRAME_8,
    FRAME_9,
    FRAME_10
};

class SpriteAtlas {
private:
    std::shared_ptr<sf::Texture> spriteTexture;

    std::map<Direction, std::map<PacManFrame, sf::IntRect>> pacmanSprites;
    std::map<DeathFrame, sf::IntRect> deathSprites;
    std::map<SpriteGhostType, std::map<Direction, std::map<GhostFrame, sf::IntRect>>> ghostSprites;
    std::map<int, sf::IntRect> fearSprites;

    sf::IntRect coinSprite;
    sf::IntRect fruitSprite;

    void defineAllSpriteRegions();

public:
    SpriteAtlas();
    ~SpriteAtlas() = default;

    bool loadFromFile(const std::string& filepath);

    [[nodiscard]] std::shared_ptr<sf::Texture> getTexture() const { return spriteTexture; }

    [[nodiscard]] sf::IntRect getPacManSprite(Direction dir, PacManFrame frame) const;
    [[nodiscard]] sf::IntRect getDeathSprite(DeathFrame frame) const;

    [[nodiscard]] sf::IntRect getGhostSprite(SpriteGhostType type, Direction dir, GhostFrame frame) const;
    [[nodiscard]] sf::IntRect getFearSprite(int frame) const;

    [[nodiscard]] sf::IntRect getCoinSprite() const { return coinSprite; }
    [[nodiscard]] sf::IntRect getFruitSprite() const { return fruitSprite; }
};

#endif