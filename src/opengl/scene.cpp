#include "mtao/opengl/scene.h"

namespace mtao::opengl {

    void Scene2D::drawGroup(const SceneGraph::DrawableGroup2D& group) {
        activeCamera().draw(group);
    }

    void Scene3D::drawGroup(const SceneGraph::DrawableGroup3D& group) {
        activeCamera().draw(group);
    }
    void Scene2D::drawGroups() {
        for(auto&& [a,pr]: _draw_groups) {
            auto&& [group, visible] = pr;
            if(visible) {
                drawGroup(group);
            }
        }
    }
    void Scene3D::drawGroups() {
        for(auto&& [a,pr]: _draw_groups) {
            auto&& [group, visible] = pr;
            if(visible) {
                drawGroup(group);
            }
        }
    }
}  // namespace mtao::opengl
