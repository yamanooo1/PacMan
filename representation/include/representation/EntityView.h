#ifndef PACMAN_ENTITYVIEW_H
#define PACMAN_ENTITYVIEW_H

#include "logic/EntityModel.h"
#include "logic/Observer.h"
#include <SFML/Graphics.hpp>
#include <memory>

class Camera;
class SpriteAtlas;

class EntityView: public Observer {
public:
  explicit EntityView(EntityModel* m, sf::RenderWindow& win, std::shared_ptr<Camera> cam, std::shared_ptr<SpriteAtlas> atlas)
      : model(m), window(win), camera(cam), spriteAtlas(atlas) {
  }

  ~EntityView() override = default;

  void update(GameEvent event) override {}
  virtual void updateAnimation(float deltaTime) {}
  virtual void draw() = 0;

  EntityView(const EntityView&) = delete;
  EntityView& operator=(const EntityView&) = delete;

  EntityModel* getModel() const { return model; }

protected:
  EntityModel* model;
  sf::RenderWindow& window;
  std::shared_ptr<Camera> camera;
  std::shared_ptr<SpriteAtlas> spriteAtlas;
};

#endif