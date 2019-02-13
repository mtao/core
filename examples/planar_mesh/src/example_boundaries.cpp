#include "example_boundaries.h"
#include "geometry.h"
#include <utility>
using namespace mtao::geometry::mesh::triangle;
Mesh make_box(int square_side, int square_aspect) {
    int ax = square_aspect * square_side;
    int ay = square_side;
    REAL sx = .5,sy = .5;
    if(ax < 0) {
        ax *= -1;
        std::swap(ax,ay);
        sy *= -square_aspect;
    } else {
        sx *= square_aspect;
    }
    Mesh sqr = 
            Translate(std::make_shared<Scale>(std::make_shared<Translate>(std::make_shared<Square>(5,ax,ay),1,0),sx,sy),4,0)
        .boundary_geometry();


    return combine({sqr});
}
Mesh make_circle(int circle_side) {
    auto spr2 = Sphere(3,3 * circle_side);
    spr2.set_edge_indices({{1,2}});
    Mesh sphere2 = spr2.boundary_geometry();


    return combine({sphere2});
}
Mesh make_box_circle(int square_aspect, int square_side, int circle_side) {
    auto spr = Sphere(1,circle_side);
    spr.set_edge_indices({{1,5}});
    Mesh sphere = spr.boundary_geometry();
    sphere.H = mtao::Vector<REAL,2>(0,0);
    int ax = square_aspect * square_side;
    int ay = square_side;
    REAL sx = .5,sy = .5;
    if(ax < 0) {
        ax *= -1;
        std::swap(ax,ay);
        sy *= -square_aspect;
    } else {
        sx *= square_aspect;
    }
    Mesh sqr = 
            Translate(std::make_shared<Scale>(std::make_shared<Translate>(std::make_shared<Square>(5,ax,ay),1,0),sx,sy),4,0)
        .boundary_geometry();


    return combine({sphere,sqr});
}
Mesh make_circle_circle(int circle_side) {
    auto spr = Sphere(1,circle_side);
    auto spr2 = Sphere(3,3*circle_side);
    spr2.set_edge_indices({{1,2}});
    Mesh sphere = spr.boundary_geometry();
    sphere.H = mtao::Vector<REAL,2>(0,0);
    Mesh sphere2 = spr2.boundary_geometry();


    return combine({sphere,sphere2});
}
