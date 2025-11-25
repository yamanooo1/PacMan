//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_CAMERA_H
#define PACMAN_CAMERA_H


class Camera {
private:
  // Current window dimensions
  float windowWidth;
  float windowHeight;

  // Current map dimensions (in grid units)
  float mapWidth;
  float mapHeight;

  // Scale factors (calculated from above)
  float scaleX;  // pixels per grid unit (horizontal)
  float scaleY;  // pixels per grid unit (vertical)

  // Recalculate scale factors
  void updateScale();

public:
  // Constructor - needs initial window and map size
  Camera(float winWidth, float winHeight, float mapW, float mapH);

  // Update window size (call when window is resized)
  void setWindowSize(float width, float height);

  // Update map size (call when loading a new map)
  void setMapSize(float width, float height);

  // Convert grid position to screen position
  float gridToScreenX(float gridX) const;
  float gridToScreenY(float gridY) const;

  // Convert screen position to grid position (useful for mouse input later)
  float screenToGridX(float screenX) const;
  float screenToGridY(float screenY) const;

  // Getters
  float getScaleX() const { return scaleX; }
  float getScaleY() const { return scaleY; }
  float getWindowWidth() const { return windowWidth; }
  float getWindowHeight() const { return windowHeight; }
};


#endif //PACMAN_CAMERA_H