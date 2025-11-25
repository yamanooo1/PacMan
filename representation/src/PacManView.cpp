//
// Created by yamanooo on 11/22/25.
//

#include "representation/PacManView.h"
#include "representation/Camera.h"

PacManView::PacManView(EntityModel* model, sf::RenderWindow* win, Camera* cam)
    : EntityView(model, cam), window(win) {
    // Setup temporary shape
    shape.setRadius(20.f);
    shape.setFillColor(sf::Color::Yellow);
}

void PacManView::update(GameEvent event) {
    // Handle specific events if needed
}

void PacManView::draw() {
  if (!model || !window || !camera) return;

  auto [x, y] = model->getPosition();

  // Center in grid cell
  float screenX = camera->gridToScreenX(x + 0.5f);
  float screenY = camera->gridToScreenY(y + 0.5f);

  // Size based on grid cell
  float gridCellSize = std::min(camera->getScaleX(), camera->getScaleY());
  float desiredSize = gridCellSize * 0.8f;  // 80% of cell
  float radius = desiredSize / 2.f;

  shape.setRadius(radius);
  shape.setOrigin(radius, radius);  // Center the shape
  shape.setPosition(screenX, screenY);

  window->draw(shape);
}