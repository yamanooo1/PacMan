#ifndef PACMAN_CAMERA_H
#define PACMAN_CAMERA_H

class Camera {
private:
  float windowWidth;
  float windowHeight;
  float mapWidth;
  float mapHeight;
  float scaleX;
  float scaleY;

  void updateScale();

public:
  Camera(float winWidth, float winHeight, float mapW, float mapH);

  void setWindowSize(float width, float height);
  void setMapSize(float width, float height);

  float gridToScreenX(float gridX) const;
  float gridToScreenY(float gridY) const;
  float screenToGridX(float screenX) const;
  float screenToGridY(float screenY) const;

  float getScaleX() const { return scaleX; }
  float getScaleY() const { return scaleY; }
  float getWindowWidth() const { return windowWidth; }
  float getWindowHeight() const { return windowHeight; }
};

#endif