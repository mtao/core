#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <mtao/eigen/stack.hpp>
#include <openvdb/openvdb.h>
#include <openvdb/points/PointConversion.h>
#include <openvdb/points/PointCount.h>
#include <mtao/geometry/mesh/write_obj.hpp>


int main(int argc, char *argv[]) {
    const std::string input_filename = argv[1];
    const std::string output_filename = argv[2];
    openvdb::initialize();
    auto P = mtao::geometry::point_cloud::points_from_partio(input_filename);
    std::vector<openvdb::Vec3R> positions;
    positions.reserve(P.cols());
    for (int j = 0; j < P.cols(); ++j) {
        auto p = P.col(j);
        positions.emplace_back(openvdb::Vec3R(p.x(), p.y(), p.z()));
    }
    openvdb::points::PointAttributeVector<openvdb::Vec3R> positionsWrapper(positions);


    int pointsPerVoxel = 8;
    float voxelSize =
      openvdb::points::computeVoxelSize(positionsWrapper, pointsPerVoxel);
    openvdb::math::Transform::Ptr transform =
      openvdb::math::Transform::createLinearTransform(voxelSize);
    openvdb::points::PointDataGrid::Ptr grid =
      openvdb::points::createPointDataGrid<openvdb::points::NullCodec,
                                           openvdb::points::PointDataGrid>(positions, *transform);
    grid->setName("Points");
    // Create a VDB file object and write out the grid.
    openvdb::io::File(output_filename).write({ grid });
}
