#include "../../include/representation/core/Camera.h"

Camera::Camera(float winWidth, float winHeight, float mapW, float mapH)
    : windowWidth(winWidth)
    , windowHeight(winHeight)
    , mapWidth(mapW)
    , mapHeight(mapH)
    , scaleX(0.0f)
    , scaleY(0.0f) {
  updateScale();
}

void Camera::updateScale() {
  if (mapWidth > 0 && mapHeight > 0) {
    scaleX = windowWidth / mapWidth;
    scaleY = windowHeight / mapHeight;
  } else {
    scaleX = 1.0f;
    scaleY = 1.0f;
  }
}

void Camera::setWindowSize(float width, float height) {
  windowWidth = width;
  windowHeight = height;
  updateScale();
}

void Camera::setMapSize(float width, float height) {
  mapWidth = width;
  mapHeight = height;
  updateScale();
}

float Camera::gridToScreenX(float gridX) const {
  return gridX * scaleX;
}

float Camera::gridToScreenY(float gridY) const {
  return gridY * scaleY;
}

float Camera::screenToGridX(float screenX) const {
  if (scaleX > 0) {
    return screenX / scaleX;
  }
  return 0.0f;
}

float Camera::screenToGridY(float screenY) const {
  if (scaleY > 0) {
    return screenY / scaleY;
  }
  return 0.0f;
}