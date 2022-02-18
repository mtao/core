#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include "particles.hpp"
#include <mtao/eigen/slice.hpp>
#include <mtao/geometry/mesh/write_obj.hpp>
Particles::Particles(const std::filesystem::path &filepath) {
    mtao::geometry::point_cloud::PartioFileReader pfr(filepath);
    densities = pfr.densities();
    positions = pfr.positions();
    velocities = pfr.velocities();
}

mtao::ColVecs3d Particles::velocities_from_indices(const std::vector<int> &P) const {
    return mtao::eigen::col_slice(velocities, P);
}
mtao::ColVecs3d Particles::positions_from_indices(const std::vector<int> &P) const {
    return mtao::eigen::col_slice(positions, P);
}

mtao::VecXd Particles::densities_from_indices(const std::vector<int> &P) const {
    return mtao::eigen::row_slice(densities, P);
}
void Particles::save_subset(const std::filesystem::path &path, const std::vector<int> &P) const {

    mtao::geometry::point_cloud::PartioFileWriter writer(path);
    writer.set_positions(positions_from_indices(P));
    writer.set_velocities(velocities_from_indices(P));
    writer.set_densities(densities_from_indices(P));
}

void Particles::save_subset_obj(const std::filesystem::path &path, const std::vector<int> &indices) const {

    auto P = positions_from_indices(indices);
    auto D = densities_from_indices(indices);
    auto B = (D.array() > .5).eval();
    mtao::ColVecs3d V(3, B.count());
    int index = 0;
    for (int j = 0; j < B.size(); ++j) {
        if (B(j)) {
            V.col(index++) = P.col(j);
        }
    }

    mtao::geometry::mesh::write_objD(V, path);
}
