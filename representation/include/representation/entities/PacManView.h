#ifndef PACMAN_PACMANVIEW_H
#define PACMAN_PACMANVIEW_H

#include "../core/SpriteAtlas.h"
#include "EntityView.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

class PacManView : public EntityView {
private:
    sf::RectangleShape shape;

    float animationTimer;
    int currentMouthFrame;
    float frameDuration;

    bool playingDeathAnimation;
    int deathFrame;
    float deathAnimationTimer;
    float deathFrameDuration;

public:
    PacManView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
               std::shared_ptr<SpriteAtlas> atlas);

    void update(logic::GameEvent event) override;
    void updateAnimation(float deltaTime) override;
    void draw() override;
};

} // namespace representation

#endif