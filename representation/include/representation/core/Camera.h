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

    [[nodiscard]] float gridToScreenX(float gridX) const;
    [[nodiscard]] float gridToScreenY(float gridY) const;
    [[nodiscard]] float screenToGridX(float screenX) const;
    [[nodiscard]] float screenToGridY(float screenY) const;

    [[nodiscard]] float getScaleX() const { return scaleX; }
    [[nodiscard]] float getScaleY() const { return scaleY; }
    [[nodiscard]] float getWindowWidth() const { return windowWidth; }
    [[nodiscard]] float getWindowHeight() const { return windowHeight; }
};

#endif