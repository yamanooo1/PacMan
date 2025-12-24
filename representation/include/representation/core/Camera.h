#ifndef PACMAN_CAMERA_H
#define PACMAN_CAMERA_H

namespace representation {

/**
 * @brief Camera - coordinate conversion between grid space and screen space
 *
 * Transforms between:
 * - Grid coordinates (logic): 10.5, 15.3
 * - Screen coordinates (pixels): Based on window size
 *
 * Scaling: scaleX = windowWidth / mapWidth (e.g., 800px / 21 cells = 38px/cell)
 *
 * Usage:
 * ```cpp
 * float screenX = camera->gridToScreenX(gridX);
 * sprite.setPosition(screenX, screenY);
 * ```
 */
class Camera {
private:
    float windowWidth;
    float windowHeight;
    float mapWidth;
    float mapHeight;
    float scaleX; // Pixels per grid unit
    float scaleY;

    void updateScale();

public:
    Camera(float winWidth, float winHeight, float mapW, float mapH);

    void setWindowSize(float width, float height);
    void setMapSize(float width, float height);

    [[nodiscard]] float gridToScreenX(float gridX) const;
    [[nodiscard]] float gridToScreenY(float gridY) const;
    [[nodiscard]] float screenToGridX(float screenX) const;
    [[nodiscard]] float screenToGridY(float screenY) const;

    [[nodiscard]] float getScaleX() const { return scaleX; }
    [[nodiscard]] float getScaleY() const { return scaleY; }
    [[nodiscard]] float getWindowWidth() const { return windowWidth; }
    [[nodiscard]] float getWindowHeight() const { return windowHeight; }
};

} // namespace representation

#endif