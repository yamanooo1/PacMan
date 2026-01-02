#include <utility>

#include "../../include/representation/core/Camera.h"
#include "../../include/representation/core/SpriteAtlas.h"
#include "../../include/representation/entities/FruitView.h"

namespace representation {

FruitView::FruitView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
                     std::shared_ptr<SpriteAtlas> atlas, int z)
    : EntityView(model, win, std::move(cam), std::move(atlas), z) {
    shape.setRadius(8.f);
    shape.setFillColor(sf::Color::Green);
}

void FruitView::update(logic::GameEvent event) {}

/**
 * @brief Render fruit sprite (power pellet)
 *
 * Scaling: 70% of grid cell (larger than coin, smaller than entities)
 */
void FruitView::draw() {
    if (!model || !camera)
        return;
    if (model->isDead())
        return;

    auto [x, y] = model->getPosition();
    float screenX = camera->gridToScreenX(x);
    float screenY = camera->gridToScreenY(y);

    if (spriteAtlas) {
        std::shared_ptr<sf::Texture> texture = spriteAtlas->getTexture();

        if (texture && texture->getSize().x > 0) {
            sf::Sprite sprite;
            sprite.setTexture(*texture);

            sf::IntRect rect = spriteAtlas->getFruitSprite();
            sprite.setTextureRect(rect);

            auto spriteWidth = static_cast<float>(rect.width);
            auto spriteHeight = static_cast<float>(rect.height);

            if (spriteWidth > 0 && spriteHeight > 0) {
                float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
                float desiredSize = gridCellSize * 0.7f; // 70% of cell

                float scaleX = desiredSize / spriteWidth;
                float scaleY = desiredSize / spriteHeight;

                sprite.setScale(scaleX, scaleY);
                sprite.setOrigin(spriteWidth / 2.f, spriteHeight / 2.f);
                sprite.setPosition(screenX, screenY);

                window.draw(sprite);
                return;
            }
        }
    }

    // Fallback: green circle (50% of cell)
    float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
    float desiredSize = gridCellSize * 0.5f;
    float radius = desiredSize / 2.f;

    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(screenX, screenY);

    window.draw(shape);
}

} // namespace representation