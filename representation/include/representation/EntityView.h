//
// EntityView.h - FIXED with shared_ptr<Camera>
//

#ifndef PACMAN_ENTITYVIEW_H
#define PACMAN_ENTITYVIEW_H

#include "logic/EntityModel.h"
#include "logic/Observer.h"
#include <memory>

class Camera;
class SpriteAtlas;

class EntityView: public Observer {
public:
  explicit EntityView(EntityModel* m, std::shared_ptr<Camera> cam, std::shared_ptr<SpriteAtlas> atlas)
      : model(m), camera(cam), spriteAtlas(atlas) {
    if (model) model->attach(this);
  }

  ~EntityView() override {
    if (model) model->detach(this);
  }

  // Observer pattern - receives events when they happen
  void update(GameEvent event) override {}

  // Animation update - called every frame (default does nothing)
  virtual void updateAnimation(float deltaTime) {}

  virtual void draw() = 0;

  // Prevent copying
  EntityView(const EntityView&) = delete;
  EntityView& operator=(const EntityView&) = delete;

  EntityModel* getModel() const { return model; }

protected:
  EntityModel* model;  // ✅ Raw pointer OK - non-owning, points to entity in World
  std::shared_ptr<Camera> camera;  // ✅ Shared ownership - multiple Views share one Camera
  std::shared_ptr<SpriteAtlas> spriteAtlas;  // ✅ Already correct
};

#endif //PACMAN_ENTITYVIEW_H