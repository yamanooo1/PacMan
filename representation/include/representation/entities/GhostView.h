#ifndef PACMAN_GHOSTVIEW_H
#define PACMAN_GHOSTVIEW_H

#include "../core/SpriteAtlas.h"
#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

class GhostView : public EntityView {
private:
    sf::CircleShape shape;
    SpriteGhostType spriteType;

    float animationTimer;
    int currentFrame;
    float frameDuration;

    float flashTimer;
    bool showWhite;

public:
    GhostView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
              std::shared_ptr<SpriteAtlas> atlas);

    void update(logic::GameEvent) override;
    void updateAnimation(float deltaTime) override;
    void draw() override;

    void setColor(const sf::Color& color) { shape.setFillColor(color); }
    void setSpriteType(SpriteGhostType type) { spriteType = type; }
};

} // namespace representation

#endif