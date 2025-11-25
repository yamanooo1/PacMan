//
// Created by yamanooo on 11/22/25.
//

#include "representation/CoinView.h"

CoinView::CoinView(EntityModel* model, sf::RenderWindow* win)
    : EntityView(model), window(win) {
  shape.setRadius(5.f);
  shape.setFillColor(sf::Color::White);
}

void CoinView::update(GameEvent event) {}

void CoinView::draw() {
  if (!model || !window) return;
  auto [x, y] = model->getPosition();
  shape.setPosition(x * 400 + 300, y * 400 + 300);
  window->draw(shape);
}