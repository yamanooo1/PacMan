//
// WallView.cpp - UPDATED with window reference
//

#include "representation/WallView.h"
#include "representation/Camera.h"
#include "representation/SpriteAtlas.h"

WallView::WallView(EntityModel* model, sf::RenderWindow& win, std::shared_ptr<Camera> cam,  // ✅ Reference
                   std::shared_ptr<SpriteAtlas> atlas)
    : EntityView(model, win, cam, atlas)  // ✅ Pass to base
{
  if (model) {
    shape.setSize(sf::Vector2f(model->getWidth() * 400, model->getHeight() * 400));
  }
  shape.setFillColor(sf::Color::Blue);
}

void WallView::update(GameEvent event) {}

void WallView::draw() {
  if (!model || !camera) return;

  auto [x, y] = model->getPosition();
  float screenX = camera->gridToScreenX(x);
  float screenY = camera->gridToScreenY(y);
  float screenWidth = camera->gridToScreenX(model->getWidth());
  float screenHeight = camera->gridToScreenY(model->getHeight());

  shape.setSize(sf::Vector2f(screenWidth, screenHeight));
  shape.setPosition(screenX, screenY);

  window.draw(shape);  // ✅ CHANGED
}