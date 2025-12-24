#include "../../include/representation/core/SpriteAtlas.h"
#include "../../../logic/include/logic/core/EntityModel.h"

namespace representation {

/**
 * @brief Initialize sprite atlas with empty texture
 *
 * Texture will be loaded via loadFromFile().
 */
SpriteAtlas::SpriteAtlas() : spriteTexture(std::make_shared<sf::Texture>()) {}

/**
 * @brief Load sprite sheet and define all sprite regions
 *
 * Two-phase initialization:
 * 1. Load texture from file
 * 2. Define pixel coordinates for all sprites
 */
bool SpriteAtlas::loadFromFile(const std::string& filepath) {
    if (!spriteTexture->loadFromFile(filepath)) {
        return false;
    }

    defineAllSpriteRegions();
    return true;
}

/**
 * @brief Define all sprite regions with hardcoded pixel coordinates
 *
 * Coordinates measured from spritemap.png sprite sheet.
 * Format: sf::IntRect(x, y, width, height)
 *
 * Organization:
 * - PacMan: 4 directions × 3 frames = 12 sprites
 * - Ghosts: 4 colors × 4 directions × 2 frames = 32 sprites
 * - Death: 11 animation frames
 * - Fear: 4 frames (normal + flashing)
 * - Collectables: coin, fruit (static sprites)
 */
void SpriteAtlas::defineAllSpriteRegions() {
    // PacMan sprites - RIGHT direction (3 mouth frames)
    pacmanSprites[logic::Direction::RIGHT][PacManFrame::CLOSED] = sf::IntRect(853, 5, 33, 33);
    pacmanSprites[logic::Direction::RIGHT][PacManFrame::HALF_OPEN] = sf::IntRect(853, 55, 33, 33);
    pacmanSprites[logic::Direction::RIGHT][PacManFrame::FULL_OPEN] = sf::IntRect(853, 105, 33, 33);

    // PacMan sprites - DOWN direction
    pacmanSprites[logic::Direction::DOWN][PacManFrame::CLOSED] = sf::IntRect(852, 155, 33, 33);
    pacmanSprites[logic::Direction::DOWN][PacManFrame::HALF_OPEN] = sf::IntRect(852, 205, 33, 33);
    pacmanSprites[logic::Direction::DOWN][PacManFrame::FULL_OPEN] = sf::IntRect(852, 255, 33, 33);

    // PacMan sprites - LEFT direction
    pacmanSprites[logic::Direction::LEFT][PacManFrame::CLOSED] = sf::IntRect(853, 305, 33, 33);
    pacmanSprites[logic::Direction::LEFT][PacManFrame::HALF_OPEN] = sf::IntRect(853, 355, 33, 33);
    pacmanSprites[logic::Direction::LEFT][PacManFrame::FULL_OPEN] = sf::IntRect(853, 405, 33, 33);

    // PacMan sprites - UP direction
    pacmanSprites[logic::Direction::UP][PacManFrame::CLOSED] = sf::IntRect(853, 454, 33, 33);
    pacmanSprites[logic::Direction::UP][PacManFrame::HALF_OPEN] = sf::IntRect(853, 504, 33, 33);
    pacmanSprites[logic::Direction::UP][PacManFrame::FULL_OPEN] = sf::IntRect(853, 554, 33, 33);

    // PacMan sprites - NONE direction (default to right closed)
    pacmanSprites[logic::Direction::NONE][PacManFrame::CLOSED] = sf::IntRect(853, 5, 33, 33);
    pacmanSprites[logic::Direction::NONE][PacManFrame::HALF_OPEN] = sf::IntRect(853, 5, 33, 33);
    pacmanSprites[logic::Direction::NONE][PacManFrame::FULL_OPEN] = sf::IntRect(853, 5, 33, 33);

    // Death animation - 11 frames (0-10)
    deathSprites[DeathFrame::FRAME_0] = sf::IntRect(351, 7, 33, 33);
    deathSprites[DeathFrame::FRAME_1] = sf::IntRect(351, 57, 33, 33);
    deathSprites[DeathFrame::FRAME_2] = sf::IntRect(351, 107, 33, 33);
    deathSprites[DeathFrame::FRAME_3] = sf::IntRect(351, 157, 33, 33);
    deathSprites[DeathFrame::FRAME_4] = sf::IntRect(351, 207, 33, 33);
    deathSprites[DeathFrame::FRAME_5] = sf::IntRect(351, 257, 33, 33);
    deathSprites[DeathFrame::FRAME_6] = sf::IntRect(351, 307, 33, 33);
    deathSprites[DeathFrame::FRAME_7] = sf::IntRect(351, 357, 33, 33);
    deathSprites[DeathFrame::FRAME_8] = sf::IntRect(351, 407, 33, 33);
    deathSprites[DeathFrame::FRAME_9] = sf::IntRect(351, 457, 33, 33);
    deathSprites[DeathFrame::FRAME_10] = sf::IntRect(351, 517, 33, 33);

    // Ghost sprites - RED ghost (all directions, 2 frames each)
    ghostSprites[SpriteGhostType::RED][logic::Direction::RIGHT][GhostFrame::FRAME_1] = sf::IntRect(1, 4, 35, 35);
    ghostSprites[SpriteGhostType::RED][logic::Direction::RIGHT][GhostFrame::FRAME_2] = sf::IntRect(1, 54, 35, 35);
    ghostSprites[SpriteGhostType::RED][logic::Direction::DOWN][GhostFrame::FRAME_1] = sf::IntRect(1, 104, 35, 35);
    ghostSprites[SpriteGhostType::RED][logic::Direction::DOWN][GhostFrame::FRAME_2] = sf::IntRect(1, 154, 35, 35);
    ghostSprites[SpriteGhostType::RED][logic::Direction::LEFT][GhostFrame::FRAME_1] = sf::IntRect(1, 204, 35, 35);
    ghostSprites[SpriteGhostType::RED][logic::Direction::LEFT][GhostFrame::FRAME_2] = sf::IntRect(1, 254, 35, 35);
    ghostSprites[SpriteGhostType::RED][logic::Direction::UP][GhostFrame::FRAME_1] = sf::IntRect(1, 304, 35, 35);
    ghostSprites[SpriteGhostType::RED][logic::Direction::UP][GhostFrame::FRAME_2] = sf::IntRect(1, 354, 35, 35);

    // Ghost sprites - PINK ghost
    ghostSprites[SpriteGhostType::PINK][logic::Direction::RIGHT][GhostFrame::FRAME_1] = sf::IntRect(51, 4, 35, 35);
    ghostSprites[SpriteGhostType::PINK][logic::Direction::RIGHT][GhostFrame::FRAME_2] = sf::IntRect(51, 54, 35, 35);
    ghostSprites[SpriteGhostType::PINK][logic::Direction::DOWN][GhostFrame::FRAME_1] = sf::IntRect(51, 104, 35, 35);
    ghostSprites[SpriteGhostType::PINK][logic::Direction::DOWN][GhostFrame::FRAME_2] = sf::IntRect(51, 154, 35, 35);
    ghostSprites[SpriteGhostType::PINK][logic::Direction::LEFT][GhostFrame::FRAME_1] = sf::IntRect(51, 204, 35, 35);
    ghostSprites[SpriteGhostType::PINK][logic::Direction::LEFT][GhostFrame::FRAME_2] = sf::IntRect(51, 254, 35, 35);
    ghostSprites[SpriteGhostType::PINK][logic::Direction::UP][GhostFrame::FRAME_1] = sf::IntRect(51, 304, 35, 35);
    ghostSprites[SpriteGhostType::PINK][logic::Direction::UP][GhostFrame::FRAME_2] = sf::IntRect(51, 354, 35, 35);

    // Ghost sprites - CYAN ghost
    ghostSprites[SpriteGhostType::CYAN][logic::Direction::RIGHT][GhostFrame::FRAME_1] = sf::IntRect(101, 4, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][logic::Direction::RIGHT][GhostFrame::FRAME_2] = sf::IntRect(101, 54, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][logic::Direction::DOWN][GhostFrame::FRAME_1] = sf::IntRect(101, 104, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][logic::Direction::DOWN][GhostFrame::FRAME_2] = sf::IntRect(101, 154, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][logic::Direction::LEFT][GhostFrame::FRAME_1] = sf::IntRect(101, 204, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][logic::Direction::LEFT][GhostFrame::FRAME_2] = sf::IntRect(101, 254, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][logic::Direction::UP][GhostFrame::FRAME_1] = sf::IntRect(101, 304, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][logic::Direction::UP][GhostFrame::FRAME_2] = sf::IntRect(101, 354, 35, 35);

    // Ghost sprites - ORANGE ghost
    ghostSprites[SpriteGhostType::ORANGE][logic::Direction::RIGHT][GhostFrame::FRAME_1] = sf::IntRect(151, 4, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][logic::Direction::RIGHT][GhostFrame::FRAME_2] = sf::IntRect(151, 54, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][logic::Direction::DOWN][GhostFrame::FRAME_1] = sf::IntRect(151, 104, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][logic::Direction::DOWN][GhostFrame::FRAME_2] = sf::IntRect(151, 154, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][logic::Direction::LEFT][GhostFrame::FRAME_1] = sf::IntRect(151, 204, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][logic::Direction::LEFT][GhostFrame::FRAME_2] = sf::IntRect(151, 254, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][logic::Direction::UP][GhostFrame::FRAME_1] = sf::IntRect(151, 304, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][logic::Direction::UP][GhostFrame::FRAME_2] = sf::IntRect(151, 354, 35, 35);

    // Fear mode sprites - blue vulnerable state
    fearSprites[0] = sf::IntRect(1, 554, 35, 35);  // Frame 0: normal blue
    fearSprites[1] = sf::IntRect(1, 604, 35, 35);  // Frame 1: normal blue (alternate)
    fearSprites[2] = sf::IntRect(51, 554, 35, 35); // Frame 2: flashing white
    fearSprites[3] = sf::IntRect(51, 604, 35, 35); // Frame 3: flashing white (alternate)

    // Collectable sprites - static (no animation)
    coinSprite = sf::IntRect(411, 313, 16, 16);  // Small white pellet
    fruitSprite = sf::IntRect(601, 200, 33, 42); // Power pellet / cherry
}

/**
 * @brief Retrieve PacMan sprite with fallback
 *
 * Returns default sprite (right closed) if lookup fails.
 */
sf::IntRect SpriteAtlas::getPacManSprite(logic::Direction dir, PacManFrame frame) const {
    auto dirIt = pacmanSprites.find(dir);
    if (dirIt != pacmanSprites.end()) {
        auto frameIt = dirIt->second.find(frame);
        if (frameIt != dirIt->second.end()) {
            return frameIt->second;
        }
    }
    // Fallback: right closed
    return {853, 5, 33, 33};
}

/**
 * @brief Retrieve death animation sprite with fallback
 */
sf::IntRect SpriteAtlas::getDeathSprite(DeathFrame frame) const {
    auto it = deathSprites.find(frame);
    if (it != deathSprites.end()) {
        return it->second;
    }
    // Fallback: first death frame
    return {351, 7, 33, 33};
}

/**
 * @brief Retrieve ghost sprite with fallback
 *
 * Returns red ghost right frame 1 if lookup fails.
 */
sf::IntRect SpriteAtlas::getGhostSprite(SpriteGhostType type, logic::Direction dir, GhostFrame frame) const {
    auto typeIt = ghostSprites.find(type);
    if (typeIt != ghostSprites.end()) {
        auto dirIt = typeIt->second.find(dir);
        if (dirIt != typeIt->second.end()) {
            auto frameIt = dirIt->second.find(frame);
            if (frameIt != dirIt->second.end()) {
                return frameIt->second;
            }
        }
    }
    // Fallback: red ghost right frame 1
    return {1, 4, 35, 35};
}

/**
 * @brief Retrieve fear mode sprite with fallback
 */
sf::IntRect SpriteAtlas::getFearSprite(int frame) const {
    auto it = fearSprites.find(frame);
    if (it != fearSprites.end()) {
        return it->second;
    }
    // Fallback: first fear frame
    return {1, 554, 35, 35};
}

} // namespace representation