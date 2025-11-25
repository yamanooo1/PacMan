//
// Created by yamanooo on 11/22/25.
//

#include "representation/Camera.h"

// Constructor - initialize with window and map dimensions
Camera::Camera(float winWidth, float winHeight, float mapW, float mapH)
    : windowWidth(winWidth)
    , windowHeight(winHeight)
    , mapWidth(mapW)
    , mapHeight(mapH)
    , scaleX(0.0f)
    , scaleY(0.0f) {

  // Calculate initial scale
  updateScale();
}

// Recalculate scale factors based on current window and map size
void Camera::updateScale() {
  // Avoid division by zero
  if (mapWidth > 0 && mapHeight > 0) {
    scaleX = windowWidth / mapWidth;
    scaleY = windowHeight / mapHeight;
  } else {
    scaleX = 1.0f;
    scaleY = 1.0f;
  }
}

// Update window size (e.g., when user resizes window)
void Camera::setWindowSize(float width, float height) {
  windowWidth = width;
  windowHeight = height;
  updateScale();  // Recalculate scale factors
}

// Update map size (e.g., when loading a new map)
void Camera::setMapSize(float width, float height) {
  mapWidth = width;
  mapHeight = height;
  updateScale();  // Recalculate scale factors
}

// Convert grid X coordinate to screen X coordinate
float Camera::gridToScreenX(float gridX) const {
  return gridX * scaleX;
}

// Convert grid Y coordinate to screen Y coordinate
float Camera::gridToScreenY(float gridY) const {
  return gridY * scaleY;
}

// Convert screen X coordinate back to grid X coordinate
float Camera::screenToGridX(float screenX) const {
  if (scaleX > 0) {
    return screenX / scaleX;
  }
  return 0.0f;
}

// Convert screen Y coordinate back to grid Y coordinate
float Camera::screenToGridY(float screenY) const {
  if (scaleY > 0) {
    return screenY / scaleY;
  }
  return 0.0f;
}