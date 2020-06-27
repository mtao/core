#pragma once

#include "mtao/opengl/objects/types.h"
#include <memory>

namespace mtao::opengl {

    // a scene object that is also its own root
    // this way scenes can be elements in higher order scenes
class Scene2D : public Magnum::SceneGraph::Scene<MatTransform2D> {
    public:
    // position in 2D space with respect to the root node
    Magnum::Vector2 localPosition(const Magnum::Vector2i& position) const;

    virtual SceneGraph::Camera2D& activeCamera() = 0;

    void drawGroup(const SceneGraph::DrawableGroup2D& group);
    void drawGroups();
    std::map<std::string,std::tuple<DrawableGroup2D,bool>> _draw_groups;

};

class Scene3D : public Magnum::SceneGraph::Scene<MatTransform3D> {
    virtual SceneGraph::Camera3D& activeCamera() = 0;



    void drawGroup(const SceneGraph::DrawableGroup3D& group);
    void drawGroups();
    std::map<std::string,std::tuple<DrawableGroup3D,bool>> _draw_groups;
};

}  // namespace mtao::opengl
