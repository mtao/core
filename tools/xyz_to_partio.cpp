#include <mtao/geometry/point_cloud/read_xyz.hpp>
#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <mtao/geometry/mesh/shapes/sphere.hpp>
#include <mtao/geometry/mesh/write_ply.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>
#include <mtao/eigen/stack.hpp>
#include <iostream>


int main(int argc, char *argv[]) {
    auto [V, N] = mtao::geometry::point_cloud::read_xyzD(argv[1]);

    bool use_color = N.size() > 0;
    int size = V.cols();


    mtao::geometry::point_cloud::PartioFileWriter w(argv[2]);
    w.set_positions(V);
    if (use_color) {
        w.set_colors(mtao::eigen::vstack(N, mtao::RowVecXd::Ones(size)));
    }

    if (argc > 3) {
        double scale = std::stof(argv[3]);
        w.set_radii(mtao::VecXd::Constant(size, scale));
    }


    return 0;
}
