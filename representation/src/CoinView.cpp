//
// Created by yamanooo on 11/22/25.
//

#include "representation/CoinView.h"
#include "representation/Camera.h"
CoinView::CoinView(EntityModel* model, sf::RenderWindow* win, Camera* cam)
    : EntityView(model, cam), window(win) {
  shape.setRadius(5.f);
  shape.setFillColor(sf::Color::White);
}

void CoinView::update(GameEvent event) {}

void CoinView::draw() {
  if (!model || !window || !camera) return;

  auto [x, y] = model->getPosition();

  float screenX = camera->gridToScreenX(x + 0.5f);
  float screenY = camera->gridToScreenY(y + 0.5f);

  float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
  float desiredSize = gridCellSize * 0.3f;
  float radius = desiredSize / 2.f;

  shape.setRadius(radius);
  shape.setOrigin(radius, radius);
  shape.setPosition(screenX, screenY);

  window->draw(shape);
}