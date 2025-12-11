#include "../../include/representation/core/SpriteAtlas.h"
#include "../../../logic/include/logic/core/EntityModel.h"

SpriteAtlas::SpriteAtlas()
    : spriteTexture(std::make_shared<sf::Texture>())
{
}

bool SpriteAtlas::loadFromFile(const std::string& filepath) {
    if (!spriteTexture->loadFromFile(filepath)) {
        return false;
    }

    defineAllSpriteRegions();

    return true;
}

void SpriteAtlas::defineAllSpriteRegions() {
    pacmanSprites[Direction::RIGHT][PacManFrame::CLOSED] = sf::IntRect(853, 5, 33, 33);
    pacmanSprites[Direction::RIGHT][PacManFrame::HALF_OPEN] = sf::IntRect(853, 55, 33, 33);
    pacmanSprites[Direction::RIGHT][PacManFrame::FULL_OPEN] = sf::IntRect(853, 105, 33, 33);

    pacmanSprites[Direction::DOWN][PacManFrame::CLOSED] = sf::IntRect(852, 155, 33, 33);
    pacmanSprites[Direction::DOWN][PacManFrame::HALF_OPEN] = sf::IntRect(852, 205, 33, 33);
    pacmanSprites[Direction::DOWN][PacManFrame::FULL_OPEN] = sf::IntRect(852, 255, 33, 33);

    pacmanSprites[Direction::LEFT][PacManFrame::CLOSED] = sf::IntRect(853, 305, 33, 33);
    pacmanSprites[Direction::LEFT][PacManFrame::HALF_OPEN] = sf::IntRect(853, 355, 33, 33);
    pacmanSprites[Direction::LEFT][PacManFrame::FULL_OPEN] = sf::IntRect(853, 405, 33, 33);

    pacmanSprites[Direction::UP][PacManFrame::CLOSED] = sf::IntRect(853, 454, 33, 33);
    pacmanSprites[Direction::UP][PacManFrame::HALF_OPEN] = sf::IntRect(853, 504, 33, 33);
    pacmanSprites[Direction::UP][PacManFrame::FULL_OPEN] = sf::IntRect(853, 554, 33, 33);

    pacmanSprites[Direction::NONE][PacManFrame::CLOSED] = sf::IntRect(853, 5, 33, 33);
    pacmanSprites[Direction::NONE][PacManFrame::HALF_OPEN] = sf::IntRect(853, 5, 33, 33);
    pacmanSprites[Direction::NONE][PacManFrame::FULL_OPEN] = sf::IntRect(853, 5, 33, 33);

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

    ghostSprites[SpriteGhostType::RED][Direction::RIGHT][GhostFrame::FRAME_1] = sf::IntRect(1, 4, 35, 35);
    ghostSprites[SpriteGhostType::RED][Direction::RIGHT][GhostFrame::FRAME_2] = sf::IntRect(1, 54, 35, 35);
    ghostSprites[SpriteGhostType::RED][Direction::DOWN][GhostFrame::FRAME_1] = sf::IntRect(1, 104, 35, 35);
    ghostSprites[SpriteGhostType::RED][Direction::DOWN][GhostFrame::FRAME_2] = sf::IntRect(1, 154, 35, 35);
    ghostSprites[SpriteGhostType::RED][Direction::LEFT][GhostFrame::FRAME_1] = sf::IntRect(1, 204, 35, 35);
    ghostSprites[SpriteGhostType::RED][Direction::LEFT][GhostFrame::FRAME_2] = sf::IntRect(1, 254, 35, 35);
    ghostSprites[SpriteGhostType::RED][Direction::UP][GhostFrame::FRAME_1] = sf::IntRect(1, 304, 35, 35);
    ghostSprites[SpriteGhostType::RED][Direction::UP][GhostFrame::FRAME_2] = sf::IntRect(1, 354, 35, 35);

    ghostSprites[SpriteGhostType::PINK][Direction::RIGHT][GhostFrame::FRAME_1] = sf::IntRect(51, 4, 35, 35);
    ghostSprites[SpriteGhostType::PINK][Direction::RIGHT][GhostFrame::FRAME_2] = sf::IntRect(51, 54, 35, 35);
    ghostSprites[SpriteGhostType::PINK][Direction::DOWN][GhostFrame::FRAME_1] = sf::IntRect(51, 104, 35, 35);
    ghostSprites[SpriteGhostType::PINK][Direction::DOWN][GhostFrame::FRAME_2] = sf::IntRect(51, 154, 35, 35);
    ghostSprites[SpriteGhostType::PINK][Direction::LEFT][GhostFrame::FRAME_1] = sf::IntRect(51, 204, 35, 35);
    ghostSprites[SpriteGhostType::PINK][Direction::LEFT][GhostFrame::FRAME_2] = sf::IntRect(51, 254, 35, 35);
    ghostSprites[SpriteGhostType::PINK][Direction::UP][GhostFrame::FRAME_1] = sf::IntRect(51, 304, 35, 35);
    ghostSprites[SpriteGhostType::PINK][Direction::UP][GhostFrame::FRAME_2] = sf::IntRect(51, 354, 35, 35);

    ghostSprites[SpriteGhostType::CYAN][Direction::RIGHT][GhostFrame::FRAME_1] = sf::IntRect(101, 4, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][Direction::RIGHT][GhostFrame::FRAME_2] = sf::IntRect(101, 54, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][Direction::DOWN][GhostFrame::FRAME_1] = sf::IntRect(101, 104, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][Direction::DOWN][GhostFrame::FRAME_2] = sf::IntRect(101, 154, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][Direction::LEFT][GhostFrame::FRAME_1] = sf::IntRect(101, 204, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][Direction::LEFT][GhostFrame::FRAME_2] = sf::IntRect(101, 254, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][Direction::UP][GhostFrame::FRAME_1] = sf::IntRect(101, 304, 35, 35);
    ghostSprites[SpriteGhostType::CYAN][Direction::UP][GhostFrame::FRAME_2] = sf::IntRect(101, 354, 35, 35);

    ghostSprites[SpriteGhostType::ORANGE][Direction::RIGHT][GhostFrame::FRAME_1] = sf::IntRect(151, 4, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][Direction::RIGHT][GhostFrame::FRAME_2] = sf::IntRect(151, 54, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][Direction::DOWN][GhostFrame::FRAME_1] = sf::IntRect(151, 104, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][Direction::DOWN][GhostFrame::FRAME_2] = sf::IntRect(151, 154, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][Direction::LEFT][GhostFrame::FRAME_1] = sf::IntRect(151, 204, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][Direction::LEFT][GhostFrame::FRAME_2] = sf::IntRect(151, 254, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][Direction::UP][GhostFrame::FRAME_1] = sf::IntRect(151, 304, 35, 35);
    ghostSprites[SpriteGhostType::ORANGE][Direction::UP][GhostFrame::FRAME_2] = sf::IntRect(151, 354, 35, 35);

    fearSprites[0] = sf::IntRect(1, 554, 35, 35);
    fearSprites[1] = sf::IntRect(1, 604, 35, 35);

    fearSprites[2] = sf::IntRect(51, 554, 35, 35);
    fearSprites[3] = sf::IntRect(51, 604, 35, 35);

    coinSprite = sf::IntRect(411, 313, 16, 16);
    fruitSprite = sf::IntRect(601, 200, 33, 42);
}

sf::IntRect SpriteAtlas::getPacManSprite(Direction dir, PacManFrame frame) const {
    auto dirIt = pacmanSprites.find(dir);
    if (dirIt != pacmanSprites.end()) {
        auto frameIt = dirIt->second.find(frame);
        if (frameIt != dirIt->second.end()) {
            return frameIt->second;
        }
    }

    return {853, 5, 33, 33};
}

sf::IntRect SpriteAtlas::getDeathSprite(DeathFrame frame) const {
    auto it = deathSprites.find(frame);
    if (it != deathSprites.end()) {
        return it->second;
    }
    return {351, 7, 33, 33};
}

sf::IntRect SpriteAtlas::getGhostSprite(SpriteGhostType type, Direction dir, GhostFrame frame) const {
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

    return {1, 4, 35, 35};
}

sf::IntRect SpriteAtlas::getFearSprite(int frame) const {
    auto it = fearSprites.find(frame);
    if (it != fearSprites.end()) {
        return it->second;
    }
    return {1, 554, 35, 35};
}