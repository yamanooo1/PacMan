//
// Created by yamanooo on 11/22/25.
//

#include "representation/PacManView.h"

PacManView::PacManView(EntityModel* model, sf::RenderWindow* win)
    : EntityView(model), window(win) {
    // Setup temporary shape
    shape.setRadius(20.f);
    shape.setFillColor(sf::Color::Yellow);
}

void PacManView::update(GameEvent event) {
    // Handle specific events if needed
}

void PacManView::draw() {
    if (!model || !window) return;

    auto [x, y] = model->getPosition();
    // Scale coordinates for now (will use Camera later)
    shape.setPosition(x * 400 + 300, y * 400 + 300);
    window->draw(shape);
}