//
// EntityView.h - FIXED with separate animation update
//

#ifndef PACMAN_ENTITYVIEW_H
#define PACMAN_ENTITYVIEW_H

#include "logic/EntityModel.h"
#include "logic/Observer.h"
#include <memory>

class Camera;
class SpriteAtlas;

class EntityView: public Observer {
private:
public:
  explicit EntityView(EntityModel* m, Camera* cam, std::shared_ptr<SpriteAtlas> atlas)
      : model(m), camera(cam), spriteAtlas(atlas) {
    if (model) model->attach(this);
  }

  ~EntityView() override {
    if (model) model->detach(this);
  }

  // ✅ Observer pattern - receives events when they happen
  void update(GameEvent event) override {}

  // ✅ NEW: Animation update - called every frame
  virtual void updateAnimation(float deltaTime) {}

  virtual void draw() = 0;

  // Prevent copying (views shouldn't be copied)
  EntityView(const EntityView&) = delete;
  EntityView& operator=(const EntityView&) = delete;

  EntityModel* getModel() const { return model; }

protected:
  EntityModel* model;
  Camera* camera;
  std::shared_ptr<SpriteAtlas> spriteAtlas;
};

#endif //PACMAN_ENTITYVIEW_H