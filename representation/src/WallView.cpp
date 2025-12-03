//
// WallView.cpp - UPDATED constructor
//

#include "representation/WallView.h"
#include "representation/Camera.h"
#include "representation/SpriteAtlas.h"  // ✅ ADD THIS

WallView::WallView(EntityModel* model, sf::RenderWindow* win, Camera* cam,
                   std::shared_ptr<SpriteAtlas> atlas)  // ✅ UPDATED
    : EntityView(model, cam, atlas)  // ✅ UPDATED
    , window(win)
{
  if (model) {
    shape.setSize(sf::Vector2f(model->getWidth() * 400, model->getHeight() * 400));
  }
  shape.setFillColor(sf::Color::Blue);
}

// Rest stays the same
void WallView::update(GameEvent event) {}

void WallView::draw() {
  if (!model || !window || !camera) return;

  auto [x, y] = model->getPosition();
  float screenX = camera->gridToScreenX(x);
  float screenY = camera->gridToScreenY(y);
  float screenWidth = camera->gridToScreenX(model->getWidth());
  float screenHeight = camera->gridToScreenY(model->getHeight());

  shape.setSize(sf::Vector2f(screenWidth, screenHeight));
  shape.setPosition(screenX, screenY);

  window->draw(shape);
}