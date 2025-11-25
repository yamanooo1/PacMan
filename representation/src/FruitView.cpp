//
// Created by yamanooo on 11/22/25.
//

#include "representation/FruitView.h"

FruitView::FruitView(EntityModel* model, sf::RenderWindow* win)
    : EntityView(model), window(win) {
  shape.setRadius(8.f);
  shape.setFillColor(sf::Color::Green);
}

void FruitView::update(GameEvent event) {}

void FruitView::draw() {
  if (!model || !window) return;
  auto [x, y] = model->getPosition();
  shape.setPosition(x * 400 + 300, y * 400 + 300);
  window->draw(shape);
}