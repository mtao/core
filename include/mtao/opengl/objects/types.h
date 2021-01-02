#pragma once
#include <Magnum/SceneGraph/MatrixTransformation2D.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/GL/Mesh.h>


namespace mtao::opengl {
using MatTransform2D = Magnum::SceneGraph::MatrixTransformation2D;
using Scene2D = Magnum::SceneGraph::Scene<MatTransform2D>;
using Object2D = Magnum::SceneGraph::Object<MatTransform2D>;

using MatTransform3D = Magnum::SceneGraph::MatrixTransformation3D;
using Scene3D = Magnum::SceneGraph::Scene<MatTransform3D>;
using Object3D = Magnum::SceneGraph::Object<MatTransform3D>;

}// namespace mtao::opengl
