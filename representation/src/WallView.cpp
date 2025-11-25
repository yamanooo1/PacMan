//
// Created by yamanooo on 11/22/25.
//

#include "representation/WallView.h"
#include "representation/Camera.h"
WallView::WallView(EntityModel* model, sf::RenderWindow* win, Camera* cam)
    : EntityView(model, cam), window(win) {
  if (model) {
    shape.setSize(sf::Vector2f(model->getWidth() * 400, model->getHeight() * 400));
  }
  shape.setFillColor(sf::Color::Blue);
}

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