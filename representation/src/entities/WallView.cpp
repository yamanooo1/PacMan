#include <utility>

#include "../../include/representation/core/Camera.h"
#include "../../include/representation/core/SpriteAtlas.h"
#include "../../include/representation/entities/WallView.h"

namespace representation {

WallView::WallView(logic::EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
                   std::shared_ptr<SpriteAtlas> atlas)
    : EntityView(model, win, std::move(cam), std::move(atlas)) {
    if (model) {
        shape.setSize(sf::Vector2f(model->getWidth() * 400, model->getHeight() * 400));
    }
    shape.setFillColor(sf::Color::Blue);
}

void WallView::update(logic::GameEvent event) {}

void WallView::draw() {
    if (!model || !camera)
        return;

    auto [x, y] = model->getPosition();
    float screenX = camera->gridToScreenX(x);
    float screenY = camera->gridToScreenY(y);
    float screenWidth = camera->gridToScreenX(model->getWidth());
    float screenHeight = camera->gridToScreenY(model->getHeight());

    shape.setSize(sf::Vector2f(screenWidth, screenHeight));
    shape.setPosition(screenX, screenY);

    window.draw(shape);
}

} // namespace representation