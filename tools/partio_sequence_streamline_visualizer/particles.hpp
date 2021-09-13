#pragma once
#include <mtao/types.hpp>
#include <filesystem>

struct Particles {
    mtao::ColVecs3d positions;
    mtao::ColVecs3d velocities;
    //std::map<std::string, mtao::VecXd> values;

    mtao::VecXd densities;
    Particles() = default;
    Particles(Particles &&) = default;
    Particles &operator=(Particles &&) = default;
    Particles(const std::filesystem::path &filepath);
    mtao::ColVecs3d positions_from_indices(const std::vector<int> &P) const;
    mtao::ColVecs3d velocities_from_indices(const std::vector<int> &P) const;
    mtao::VecXd densities_from_indices(const std::vector<int> &P) const;


    void save_subset(const std::filesystem::path &path, const std::vector<int> &P) const;
    void save_subset_obj(const std::filesystem::path &path, const std::vector<int> &P) const;

    int count() const {
        return positions.cols();
    }
};
