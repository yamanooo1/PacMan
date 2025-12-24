#ifndef PACMAN_SPRITEATLAS_H
#define PACMAN_SPRITEATLAS_H

#include <SFML/Graphics.hpp>
#include <map>
#include <memory>

namespace logic {
enum class Direction;
}

namespace representation {

// Sprite frame enums for animation indexing
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

/**
 * @brief SpriteAtlas - sprite sheet texture manager with frame definitions
 *
 * Purpose:
 * - Loads single sprite sheet texture (spritemap.png)
 * - Defines rectangular regions (sf::IntRect) for each sprite frame
 * - Provides lookup methods for entity views to retrieve correct sprite
 *
 * Sprite Sheet Organization:
 * ```
 * spritemap.png:
 * ┌────────────────────────────────────┐
 * │ [Red Ghost→]  [Pink→]  [Cyan→]    │ Row 1-8: Ghost sprites
 * │ [Ghost←]      [Fear]   [Eyes]     │
 * │                                    │
 * │ [PacMan→]  [PacMan←]  [PacMan↑]   │ Row 9-16: PacMan sprites
 * │ [Death anim frames 0-10]          │
 * │                                    │
 * │ [Coin] [Fruit] [Wall tiles]       │ Row 17+: Collectables
 * └────────────────────────────────────┘
 * ```
 *
 * Coordinate System:
 * - sf::IntRect(x, y, width, height) in pixels
 * - Origin: top-left corner of sprite sheet
 * - Example: PacMan right closed = IntRect(853, 5, 33, 33)
 *
 * Animation System:
 * - PacMan: 3 frames per direction (closed, half, full open)
 * - Ghost: 2 frames per direction (alternating movement)
 * - Death: 11 frames (linear sequence)
 * - Fear: 4 frames (2 normal blue + 2 flashing white)
 *
 * Usage Pattern:
 * ```cpp
 * // In PacManView::draw():
 * auto rect = spriteAtlas->getPacManSprite(Direction::RIGHT, PacManFrame::FULL_OPEN);
 * sprite.setTextureRect(rect);
 * sprite.setTexture(*spriteAtlas->getTexture());
 * ```
 *
 * Design Rationale:
 * - Shared texture: All entities use same texture (memory efficient)
 * - Nested maps: type → direction → frame for structured lookup
 * - IntRect definitions: Hardcoded coordinates from sprite sheet analysis
 *
 * @see EntityView for sprite rendering usage
 */
class SpriteAtlas {
private:
    std::shared_ptr<sf::Texture> spriteTexture; // Single sprite sheet texture

    // Nested maps for structured sprite lookup
    std::map<logic::Direction, std::map<PacManFrame, sf::IntRect>> pacmanSprites;
    std::map<DeathFrame, sf::IntRect> deathSprites;
    std::map<SpriteGhostType, std::map<logic::Direction, std::map<GhostFrame, sf::IntRect>>> ghostSprites;
    std::map<int, sf::IntRect> fearSprites; // int: 0-3 for normal/flashing frames

    sf::IntRect coinSprite;
    sf::IntRect fruitSprite;

    /**
     * @brief Define all sprite regions (hardcoded pixel coordinates)
     *
     * Called after texture loading to populate all sprite maps.
     * Coordinates measured manually from sprite sheet.
     */
    void defineAllSpriteRegions();

public:
    SpriteAtlas();
    ~SpriteAtlas() = default;

    /**
     * @brief Load sprite sheet texture from file
     *
     * @param filepath Path to sprite sheet image (e.g., "sprites/spritemap.png")
     * @return true if loading successful
     */
    bool loadFromFile(const std::string& filepath);

    /**
     * @brief Get shared texture pointer for sprite rendering
     *
     * All entity views share this texture (efficient memory usage).
     *
     * @return Shared pointer to sprite sheet texture
     */
    [[nodiscard]] std::shared_ptr<sf::Texture> getTexture() const { return spriteTexture; }

    /**
     * @brief Get PacMan sprite region for animation
     *
     * @param dir Current movement direction
     * @param frame Mouth animation frame (closed, half, full open)
     * @return Rectangle defining sprite region in texture
     */
    [[nodiscard]] sf::IntRect getPacManSprite(logic::Direction dir, PacManFrame frame) const;

    /**
     * @brief Get death animation sprite frame
     *
     * @param frame Death animation stage (0-10)
     * @return Rectangle for death sprite
     */
    [[nodiscard]] sf::IntRect getDeathSprite(DeathFrame frame) const;

    /**
     * @brief Get ghost sprite for type, direction, and animation frame
     *
     * @param type Ghost color (RED, PINK, CYAN, ORANGE)
     * @param dir Movement direction
     * @param frame Animation frame (1 or 2)
     * @return Rectangle for ghost sprite
     */
    [[nodiscard]] sf::IntRect getGhostSprite(SpriteGhostType type, logic::Direction dir, GhostFrame frame) const;

    /**
     * @brief Get fear mode sprite (blue/flashing)
     *
     * @param frame 0-1: normal blue, 2-3: flashing white
     * @return Rectangle for fear sprite
     */
    [[nodiscard]] sf::IntRect getFearSprite(int frame) const;

    // Collectable sprite accessors
    [[nodiscard]] sf::IntRect getCoinSprite() const { return coinSprite; }
    [[nodiscard]] sf::IntRect getFruitSprite() const { return fruitSprite; }
};

} // namespace representation

#endif