//
// Created by yamanooo on 11/22/25.
//

#include "representation/PacManView.h"
#include "representation/Camera.h"

PacManView::PacManView(EntityModel* model, sf::RenderWindow* win, Camera* cam)
    : EntityView(model, cam), window(win) {
  shape.setFillColor(sf::Color::Yellow);
}

void PacManView::update(GameEvent event) {
    // Handle specific events if needed
}

void PacManView::draw() {
  if (!model || !window || !camera) return;

  auto [x, y] = model->getPosition();

  // Convert model position to screen position
  float screenX = camera->gridToScreenX(x);
  float screenY = camera->gridToScreenY(y);

  // Convert model size to screen size (using scaleX and scaleY independently)
  float screenWidth = camera->gridToScreenX(model->getWidth());
  float screenHeight = camera->gridToScreenY(model->getHeight());

  shape.setSize(sf::Vector2f(screenWidth, screenHeight));
  shape.setPosition(screenX, screenY);

  window->draw(shape);
}