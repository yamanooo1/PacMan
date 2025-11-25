//
// Created by yamanooo on 11/22/25.
//

#include "representation/WallView.h"

WallView::WallView(EntityModel* model, sf::RenderWindow* win)
    : EntityView(model), window(win) {
  if (model) {
    shape.setSize(sf::Vector2f(model->getWidth() * 400, model->getHeight() * 400));
  }
  shape.setFillColor(sf::Color::Blue);
}

void WallView::update(GameEvent event) {}

void WallView::draw() {
  if (!model || !window) return;
  auto [x, y] = model->getPosition();
  shape.setPosition(x * 400 + 300, y * 400 + 300);
  window->draw(shape);
}