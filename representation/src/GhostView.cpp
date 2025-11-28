//
// Created by yamanooo on 11/22/25.
//

#include "representation/GhostView.h"
#include "representation/Camera.h"
#include "logic/Ghost.h"

GhostView::GhostView(EntityModel* model, sf::RenderWindow* win, Camera* cam)
    : EntityView(model, cam), window(win) {
  shape.setRadius(20.f);
  shape.setFillColor(sf::Color::Red);  // Default color (will be overridden by factory)
}

void GhostView::update(GameEvent event) {}

void GhostView::draw() {
  if (!model || !window || !camera) return;

  auto [x, y] = model->getPosition();

  float centerX = x + model->getWidth() / 2.0f;
  float centerY = y + model->getHeight() / 2.0f;

  float screenX = camera->gridToScreenX(centerX);
  float screenY = camera->gridToScreenY(centerY);

  float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
  float desiredSize = gridCellSize * 0.8f;
  float radius = desiredSize / 2.f;

  shape.setRadius(radius);
  shape.setOrigin(radius, radius);
  shape.setPosition(screenX, screenY);

  // NEW: Temporarily change color for fear mode
  Ghost* ghost = static_cast<Ghost*>(model);
  sf::Color originalColor = shape.getFillColor();

  if (ghost && ghost->isInFearMode()) {
    shape.setFillColor(sf::Color::Blue);
  }

  window->draw(shape);

  // Restore original color
  shape.setFillColor(originalColor);
}