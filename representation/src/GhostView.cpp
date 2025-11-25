//
// Created by yamanooo on 11/22/25.
//

#include "representation/GhostView.h"

GhostView::GhostView(EntityModel* model, sf::RenderWindow* win)
    : EntityView(model), window(win) {
  shape.setRadius(20.f);
  shape.setFillColor(sf::Color::Red);
}

void GhostView::update(GameEvent event) {}

void GhostView::draw() {
  if (!model || !window) return;
  auto [x, y] = model->getPosition();
  shape.setPosition(x * 400 + 300, y * 400 + 300);
  window->draw(shape);
}