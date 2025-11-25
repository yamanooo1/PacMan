//
// Created by yamanooo on 11/22/25.
//

#ifndef PACMAN_ENTITYVIEW_H
#define PACMAN_ENTITYVIEW_H
#include "logic/EntityModel.h"
#include "logic/Observer.h"

class EntityView: public Observer {
private:
public:
  explicit EntityView(EntityModel* m) : model(m) {
    if (model) model ->attach(this);
  }
  ~EntityView() override {
    if (model) model->detach(this);
  }
  void update(GameEvent event) override {}

  virtual void draw() = 0;

  // Prevent copying (views shouldn't be copied)
  EntityView(const EntityView&) = delete;
  EntityView& operator=(const EntityView&) = delete;

protected:
  EntityModel* model;
};


#endif //PACMAN_ENTITYVIEW_H