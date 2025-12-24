#ifndef PACMAN_ENTITYVIEW_H
#define PACMAN_ENTITYVIEW_H

#include "../../../../logic/include/logic/core/EntityModel.h"
#include "../../../../logic/include/logic/patterns/Observer.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace representation {

class Camera;
class SpriteAtlas;

class EntityView : public logic::Observer {
public:
    explicit EntityView(logic::EntityModel* m, sf::RenderWindow& win, std::shared_ptr<Camera> cam,
                        std::shared_ptr<SpriteAtlas> atlas)
        : model(m), window(win), camera(std::move(cam)), spriteAtlas(std::move(atlas)) {}

    ~EntityView() override = default;

    void update(logic::GameEvent event) override {}
    virtual void updateAnimation(float deltaTime) {}
    virtual void draw() = 0;

    EntityView(const EntityView&) = delete;
    EntityView& operator=(const EntityView&) = delete;

    [[nodiscard]] logic::EntityModel* getModel() const { return model; }

protected:
    logic::EntityModel* model;
    sf::RenderWindow& window;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<SpriteAtlas> spriteAtlas;
};

} // namespace representation

#endif