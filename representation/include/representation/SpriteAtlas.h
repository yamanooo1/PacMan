//
// SpriteAtlas.h - Manages sprite texture and regions
// NOT a singleton - passed through factory
//

#ifndef PACMAN_SPRITEATLAS_H
#define PACMAN_SPRITEATLAS_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <map>

// Forward declaration
enum class Direction;

// Enum for PacMan animation frames
enum class PacManFrame {
    CLOSED,
    HALF_OPEN,
    FULL_OPEN
};

// Enum for Ghost animation frames
enum class GhostFrame {
    FRAME_1,
    FRAME_2
};

// Enum for Ghost types (matches your GhostType in logic)
enum class SpriteGhostType {
    RED,
    PINK,
    CYAN,
    ORANGE
};

// Enum for death animation frames
enum class DeathFrame {
    FRAME_0, FRAME_1, FRAME_2, FRAME_3, FRAME_4,
    FRAME_5, FRAME_6, FRAME_7, FRAME_8, FRAME_9, FRAME_10
};

class SpriteAtlas {
private:
    std::shared_ptr<sf::Texture> spriteTexture;  // ✅ Smart pointer

    // Store sprite regions organized by entity type
    std::map<Direction, std::map<PacManFrame, sf::IntRect>> pacmanSprites;
    std::map<DeathFrame, sf::IntRect> deathSprites;
    std::map<SpriteGhostType, std::map<Direction, std::map<GhostFrame, sf::IntRect>>> ghostSprites;
    std::map<int, sf::IntRect> fearSprites;  // 2 frames for scared ghosts

    sf::IntRect coinSprite;
    sf::IntRect fruitSprite;

    // Private helper to define all sprite regions
    void defineAllSpriteRegions();

public:
    SpriteAtlas();
    ~SpriteAtlas() = default;

    // Load the sprite texture from file
    bool loadFromFile(const std::string& filepath);

    // Get the shared texture (for sf::Sprite)
    std::shared_ptr<sf::Texture> getTexture() const { return spriteTexture; }

    // PacMan sprite getters
    sf::IntRect getPacManSprite(Direction dir, PacManFrame frame) const;
    sf::IntRect getDeathSprite(DeathFrame frame) const;

    // Ghost sprite getters
    sf::IntRect getGhostSprite(SpriteGhostType type, Direction dir, GhostFrame frame) const;
    sf::IntRect getFearSprite(int frame) const;  // frame 0 or 1

    // Collectible sprite getters
    sf::IntRect getCoinSprite() const { return coinSprite; }
    sf::IntRect getFruitSprite() const { return fruitSprite; }
};

#endif // PACMAN_SPRITEATLAS_H