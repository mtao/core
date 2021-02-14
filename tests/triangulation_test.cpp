#include <catch2/catch.hpp>

#include <mtao/geometry/mesh/triangle_fan.hpp>
#include <mtao/geometry/mesh/earclipping.hpp>
#include <mtao/geometry/volume.hpp>
#include <mtao/geometry/mesh/polygon_boundary.hpp>
#include <mtao/geometry/mesh/polygon_boundary_triangulation.hpp>
#include <mtao/geometry/mesh/polygon_boundary_centroid.hpp>


TEST_CASE("convex", "[triangulation]") {


    std::vector<int> F{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    mtao::geometry::mesh::PolygonBoundaryIndices PBI(F);
    int N = F.size();
    Eigen::VectorXd Theta = Eigen::VectorXd::LinSpaced(N, 0, (N - 1) * (2 * M_PI) / N);
    mtao::ColVecs2d P(2, Theta.size());
    P.row(0) = Theta.array().cos().transpose();
    P.row(1) = Theta.array().sin().transpose();

    auto F_TF = mtao::geometry::mesh::triangulate_fan(PBI);
    auto F_EC = mtao::geometry::mesh::triangulate_earclipping(PBI, P);
    auto F_T = mtao::geometry::mesh::triangulate_triangle(PBI, P);

    auto vol = mtao::geometry::curve_volume(P, F);

    double V_TF = mtao::geometry::volumes(P, F_TF).sum();
    double V_EC = mtao::geometry::volumes(P, F_TF).sum();
    double V_T = mtao::geometry::volumes(P, F_TF).sum();


    CHECK(V_TF == Approx(vol));
    CHECK(V_EC == Approx(vol));
    CHECK(V_T == Approx(vol));



    auto cent = mtao::geometry::mesh::centroid(PBI, P);

    CHECK(cent.x() == Approx(0.).margin(1e-5));
    CHECK(cent.y() == Approx(0.).margin(1e-5));

    cent = mtao::geometry::mesh::centroid(PBI, (P.array()+.5).matrix());

    CHECK(cent.x() == Approx(0.5).margin(1e-5));
    CHECK(cent.y() == Approx(0.5).margin(1e-5));

}
