#include <utility>

#include "../../include/representation/core/Camera.h"
#include "../../include/representation/core/SpriteAtlas.h"
#include "../../include/representation/entities/CoinView.h"

namespace representation {

CoinView::CoinView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
                   std::shared_ptr<SpriteAtlas> atlas)
    : EntityView(model, win, std::move(cam), std::move(atlas)) {
    shape.setRadius(5.f);
    shape.setFillColor(sf::Color::White);
}

void CoinView::update(logic::GameEvent event) {}

/**
 * @brief Render coin sprite centered on grid position
 *
 * Early exits:
 * - No model/camera
 * - Model is dead (collected)
 *
 * Scaling: 40% of grid cell (smaller than entities)
 */
void CoinView::draw() {
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

            sf::IntRect rect = spriteAtlas->getCoinSprite();
            sprite.setTextureRect(rect);

            auto spriteWidth = static_cast<float>(rect.width);
            auto spriteHeight = static_cast<float>(rect.height);

            if (spriteWidth > 0 && spriteHeight > 0) {
                float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
                float desiredSize = gridCellSize * 0.4f; // 40% of cell

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

    // Fallback: white circle (30% of cell)
    float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
    float desiredSize = gridCellSize * 0.3f;
    float radius = desiredSize / 2.f;

    shape.setRadius(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(screenX, screenY);

    window.draw(shape);
}

} // namespace representation