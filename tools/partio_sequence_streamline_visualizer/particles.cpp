#include <mtao/geometry/point_cloud/partio_loader.hpp>
#include "particles.hpp"
#include <mtao/eigen/slice.hpp>
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
