#pragma once
#include <mtao/geometry/mesh/triangle/mesh.h>

mtao::geometry::mesh::triangle::Mesh make_freehand(const mtao::ColVecs2d& V, const mtao::ColVecs2i& E, const mtao::ColVecs2d& H = {});
mtao::geometry::mesh::triangle::Mesh make_freehand(const std::string& filename);
mtao::geometry::mesh::triangle::Mesh make_box(int square_side, int square_aspect);
mtao::geometry::mesh::triangle::Mesh make_circle(int circle_side);
mtao::geometry::mesh::triangle::Mesh make_box_circle(int square_aspect, int square_side, int circle_side);
mtao::geometry::mesh::triangle::Mesh make_circle_circle(int circle_side);
