//
// Created by yamanooo on 11/22/25.
//

#include "representation/FruitView.h"
#include "representation/Camera.h"
FruitView::FruitView(EntityModel* model, sf::RenderWindow* win, Camera* cam)
    : EntityView(model, cam), window(win) {
  shape.setRadius(8.f);
  shape.setFillColor(sf::Color::Green);
}

void FruitView::update(GameEvent event) {}

void FruitView::draw() {
  if (!model || !window || !camera) return;
  if (model->isDead()) return;

  auto [x, y] = model->getPosition();

  float screenX = camera->gridToScreenX(x + 0.5f);
  float screenY = camera->gridToScreenY(y + 0.5f);

  float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
  float desiredSize = gridCellSize * 0.5f;
  float radius = desiredSize / 2.f;

  shape.setRadius(radius);
  shape.setOrigin(radius, radius);
  shape.setPosition(screenX, screenY);

  window->draw(shape);
}