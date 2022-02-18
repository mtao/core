#include "tube_mesh_constructor.hpp"
#include "tube_mesh_constructor.hpp"
#include <mtao/iterator/enumerate.hpp>
#include <spdlog/spdlog.h>
#include <mtao/geometry/mesh/shapes/tube.hpp>

TubeMeshConstructor::TubeMeshConstructor(
  const std::vector<Particles> &particles,

  const std::vector<int> &active_indices,
  const bool &show_all_particles) : particles(particles), active_indices(active_indices), show_all_particles(show_all_particles) {}


void TubeMeshConstructor::set_scalar_function_mode(ScalarFunctionMode mode) {
    scalar_function_mode = mode;
}

std::array<int, 2> TubeMeshConstructor::valid_interval(int index) const {

    index = std::max<int>(0, index);
    int last_index = index + 1;
    last_index = tail_size + index;
    last_index = std::min<int>(particles.size(), last_index);
    return { { index, last_index } };
}

const Particles &TubeMeshConstructor::frame_particles(int index) const {
    return particles.at(index);
}
mtao::ColVecs3d TubeMeshConstructor::frame_positions(int index) const {
    const auto &p = frame_particles(index);
    if (show_all_particles) {
        return p.positions;
    } else {
        return p.positions_from_indices(active_indices);
    }
}
mtao::ColVecs3d TubeMeshConstructor::frame_velocities(int index) const {
    const auto &p = frame_particles(index);
    if (show_all_particles) {
        return p.velocities;
    } else {
        return p.velocities_from_indices(active_indices);
    }
}
mtao::VecXd TubeMeshConstructor::frame_densities(int index) const {
    const auto &p = frame_particles(index);
    if (show_all_particles) {
        return p.densities;
    } else {
        return p.densities_from_indices(active_indices);
    }
}

std::tuple<mtao::ColVecs3d, mtao::VecXd> TubeMeshConstructor::get_pos_scalar(int index) const {
    std::tuple<mtao::ColVecs3d, mtao::VecXd> ret;
    auto &[V, F] = ret;
    V = frame_positions(index);
    F.resize(V.cols());
    switch (scalar_function_mode) {
    case ScalarFunctionMode::Selected: {

        F.setZero();
        if (show_all_particles) {

            for (int idx : active_indices) {
                F(idx) = 1;
            }
        } else {

            F.setOnes();
        }

        break;
    }
    case ScalarFunctionMode::Density: {
        F = frame_densities(index);
        break;
    }
    case ScalarFunctionMode::Velocity: {
        F = frame_velocities(index).colwise().norm().transpose();
        break;
    }
    case ScalarFunctionMode::All:
    default: {
        F.setConstant(1);
        break;
    }
    }
    return ret;
}
std::vector<std::tuple<mtao::ColVecs3d, mtao::VecXd>> TubeMeshConstructor::get_pos_scalar_tail(int index) const {
    int last_index;
    {
        auto [a, b] = valid_interval(index);
        index = a;
        last_index = b;
    }

    auto [VF, FF] = get_pos_scalar(index);
    int tail_size = last_index - index;


    std::vector<std::tuple<mtao::ColVecs3d, mtao::VecXd>> ret(VF.cols());
    for (auto &&[idx, pr] : mtao::iterator::enumerate(ret)) {
        auto &[V, F] = pr;
        V.resize(3, tail_size);
        V.col(0) = VF.col(idx);
        F.resize(tail_size);
        F(0) = FF(idx);
    }
    for (int j = 1; j < tail_size; ++j) {
        auto [VF, FF] = get_pos_scalar(index + j);
        for (auto &&[idx, pr] : mtao::iterator::enumerate(ret)) {
            auto &[V, F] = pr;
            V.col(j) = VF.col(idx);
            F(j) = FF(idx);
        }
    }

    return ret;
}

std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs2i> TubeMeshConstructor::get_pos_scalar_lines(int index) const {
    if(show_all_particles) {
        spdlog::warn("Tube mesh constructor is unwilling to generate geometry for all particles. Try deactivating show_all_particles but fully populating active_indices");
        return {};
    }
    auto tails = get_pos_scalar_tail(index);

    std::vector<mtao::ColVecs3d> tV(tails.size());
    std::vector<mtao::VecXd> tF(tails.size());
    std::vector<mtao::ColVecs2i> tE(tails.size());

    int offset = 0;
    for (auto &&[TV, TF, TE, pr] : mtao::iterator::zip(tV, tF, tE, tails)) {
        auto &[V, F] = pr;

        TV = V;
        TF = F;
        TE.resize(2, TV.cols() - 1);
        //spdlog::info("Local V,T,E sizes: {},{},{}", TV.cols(), TF.cols(), TE.cols());
        for (int j = 0; j < TE.cols(); ++j) {
            auto e = TE.col(j);
            int val = j + offset;
            e << val, val + 1;
        }

        // std::cout << TE << std::endl;
        offset += TV.cols();
    }
    std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs2i> ret;
    {
        std::get<0>(ret) = mtao::eigen::hstack_iter(tV.begin(), tV.end());
        std::get<1>(ret) = mtao::eigen::vstack_iter(tF.begin(), tF.end());
        std::get<2>(ret) = mtao::eigen::hstack_iter(tE.begin(), tE.end());
        const auto &[V, F, T] = ret;
        spdlog::info("vertices {} function {} edges {}", V.cols(), F.size(), T.cols());
    }
    return ret;
}

std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs3i> TubeMeshConstructor::get_pos_scalar_tubes(int index) const {

    auto tails = get_pos_scalar_tail(index);

    mtao::geometry::mesh::shapes::internal::TubeConstructor tc{ tube_radius, tube_subdivisions, true, true };
    std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs3i> ret;

    //std::vector<std::tuple<mtao::ColVecs3d, mtao::VecXd, mtao::ColVecs3i>> single_tubes(Vs.size());
    std::vector<mtao::ColVecs3d> tV(tails.size());
    std::vector<mtao::VecXd> tF(tails.size());
    std::vector<mtao::ColVecs3i> tT(tails.size());

    int offset = 0;
    for (auto &&[TV, TF, TT, pr] : mtao::iterator::zip(tV, tF, tT, tails)) {
        auto &[V, F] = pr;

        //std::cout << "Input Vertices\n"
        //          << V << std::endl;
        TV = tc.create_vertices(V);
        TF = tc.create_per_ring_data_rows(F);
        TT = tc.create_triangulation(V.cols());
        if (TT.maxCoeff() > TV.cols()) {
            spdlog::info("Triangle indices are bad, too big ({} > {})", TT.maxCoeff(), TV.cols());
        }
        if (TT.minCoeff() < 0) {
            spdlog::info("Triangle small index found: {}", TT.minCoeff());
        }
        TT.array() += offset;

        offset += TV.cols();
    }
    {
        std::get<0>(ret) = mtao::eigen::hstack_iter(tV.begin(), tV.end());
        std::get<1>(ret) = mtao::eigen::vstack_iter(tF.begin(), tF.end());
        std::get<2>(ret) = mtao::eigen::hstack_iter(tT.begin(), tT.end());
        const auto &[V, F, T] = ret;
        spdlog::info("vertices {} function {} triangles {}", V.cols(), F.size(), T.cols());
        //std::cout << "V:\n"
        //          << V << std::endl;
        //std::cout << "F:\n"
        //          << F << std::endl;
        //std::cout << "T:\n"
        //          << T << std::endl;
    }
    return ret;
}
