#include "mtao/geometry/mesh/shapes/tube.hpp"
namespace mtao::geometry::mesh::shapes::internal {
mtao::ColVecs3i TubeConstructor::create_triangulation(int segment_count) const {

    mtao::ColVecs3i RF(3, 2 * (segment_count - 1) * subdivisions);
    for (int i = 0; i < segment_count; ++i) {

        auto f = RF.block(0, 2 * subdivisions * i, RF.rows(), 2 * subdivisions);
        for (int j = 0; j < subdivisions; ++j) {
            int voff = subdivisions * i;
            int u = voff + j;
            int v = voff + (j + 1) % subdivisions;
            int s = voff + subdivisions + j;
            int t = voff + subdivisions + (j + 1) % subdivisions;
            f.col(2 * j) << u, v, s;
            f.col(2 * j + 1) << s, v, t;
        }
    }

    if (front || back) {
        auto CF = internal::disc_faces(subdivisions);
        if (front & back) {
            mtao::ColVecs3i CSwiz(CF.rows(), CF.cols());
            CSwiz.row(0) = CF.row(0);
            CSwiz.row(1) = CF.row(2);
            CSwiz.row(2) = CF.row(1);
            //RF = mtao::eigen::hstack(RF,CF,CF({0,2,1},Eigen::all).array()+(segment_count-1)*subdivisions);
            RF = mtao::eigen::hstack(RF, CF, CSwiz.array() + (segment_count - 1) * subdivisions);
        } else {
            if (front) {
                RF = mtao::eigen::hstack(RF, CF);
            } else {
                mtao::ColVecs3i CSwiz(CF.rows(), CF.cols());
                CSwiz.row(0) = CF.row(0);
                CSwiz.row(1) = CF.row(2);
                CSwiz.row(2) = CF.row(1);
                //RF = mtao::eigen::hstack(RF,CF({0,2,1},Eigen::all).array()+(segment_count-1)*subdivisions);
                RF = mtao::eigen::hstack(RF, CSwiz.array() + (segment_count - 1) * subdivisions);
            }
        }
    }
    return RF;
}
}// namespace mtao::geometry::mesh::shapes::internal
