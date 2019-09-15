#include "mtao/geometry/bounding_box.hpp"
#include <mtao/geometry/grid/grid.h>
#include <mtao/geometry/grid/grid_data.hpp>
#include "curve.hpp"

int main(int argc, char * argv[]) {

    Eigen::AlignedBox<float,2> bbox;
    bbox.min().setConstant(-1);
    bbox.max().setConstant(1);
    mtao::geometry::grid::Grid2f grid;
    mtao::geometry::grid::GridData2f grid_data;
    Curve curve;
    std::array<int,2> N{{5,5}};
    int& NI=N[0];
    int& NJ=N[1];
    curve.points.emplace_back(mtao::Vec2d(.8,.8));
    curve.points.emplace_back(mtao::Vec2d(.7,-.2));
    curve.points.emplace_back(mtao::Vec2d(-.8,-.2));
    curve.points.emplace_back(mtao::Vec2d(-.7,.8));
    grid = mtao::geometry::grid::Grid2f::from_bbox
        (bbox, std::array<int,2>{{NI,NJ}});
    auto V = grid.vertices();
    auto evaluator = curve.evaluator();
    for(int i = 0; i < curve.vertex_size(); ++i) {
        mtao::VecXd coeffs = mtao::VecXd::Unit(curve.vertex_size(),i);
        mtao::VecXf D = evaluator.from_coefficients(coeffs, V.cast<double>()).cast<float>();
        return 0;
    }
    return 0;
}
