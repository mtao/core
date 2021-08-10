#include "filter.hpp"
#include "mtao/opengl/objects/sphere.hpp"
#include <imgui.h>
#include <mtao/iterator/enumerate.hpp>
#include <spdlog/spdlog.h>
std::vector<int> Filter::selected_particles(const Particles &p) const {

    auto query = particle_mask(p);
    std::vector<int> indices;
    for (auto &&[idx, val] : mtao::iterator::enumerate(query)) {
        if (val) {
            indices.emplace_back(idx);
        }
    }
    return indices;
}


auto PlaneFilter::particle_mask(const Particles &p) const -> BoolVec {

    const auto &V = p.positions;
    auto P = get_equation();

    auto vals = (P.transpose() * V.cast<float>().colwise().homogeneous()).eval();
    auto query = (vals.cwiseAbs().array() < 1.f).eval();
    return query.transpose();
}
bool PlaneFilter::gui() {
    return mtao::opengl::objects::Plane::gui();
}

auto SphereFilter::particle_mask(const Particles &p) const -> BoolVec {
    const auto &V = p.positions;

    auto vals = ((V.cast<float>().colwise() - center()).colwise().norm()).eval();
    auto query = (vals.array() < radius()).eval();
    return query.transpose();
}
bool SphereFilter::gui() {

    return mtao::opengl::objects::PositionedSphere::gui();
}

MeshFilter::MeshFilter(const Eigen::MatrixXf &V, const Eigen::MatrixXi &F) : V(std::move(V)), F(std::move(F)) {

    aabb.init(V, F);
}

auto MeshFilter::particle_mask(const Particles &p) const -> BoolVec {
    Eigen::MatrixXf C;
    mtao::VecXf sqrD;
    mtao::VecXi I;


    auto P = p.positions.transpose().cast<float>().eval();
    aabb.squared_distance(V, F, P, sqrD, I, C);
    spdlog::info("Got squared dist");
    return sqrD.array() < mesh_distance * mesh_distance;
}
bool MeshFilter::gui() {
    bool changed = false;
    changed |= ImGui::InputFloat("Distance", &mesh_distance);
    return changed;
}
auto IntersectionFilter::particle_mask(const Particles &p) const -> BoolVec {
    auto it = filters.begin();
    BoolVec R = BoolVec::Constant(true, p.count());
    for (; it != filters.end(); ++it) {
        const auto &[name, filter, active] = *it;
        if (active) {
            R = R && filter->particle_mask(p);
        }
    }
    return R;
}
bool IntersectionFilter::gui() {

    bool changed = false;
    for (auto &&[name, filter, active] : filters) {
        changed |= ImGui::Checkbox(name.c_str(), &active);
    }
    return changed;
}

auto PruneFilter::particle_mask(const Particles &p) const -> BoolVec {


    mtao::VecXf vec = Eigen::VectorXf::Random(p.count()).eval();
    vec = (vec.array() + 1.f) / 2.f;
    return vec.array() < percentage;
}
bool PruneFilter::gui() {
    bool changed = false;
    changed |= ImGui::SliderFloat("Percentage", &percentage, 0.f, 1.f);
    return changed;
}

auto RangeFilter::particle_mask(const Particles &p) const -> BoolVec {

    const auto &V = p.velocities;

    auto vals = V.cast<float>().colwise().norm().transpose().eval();


    return (range[0] < vals.array()) && (vals.array() < range[1]);
}
bool RangeFilter::gui() {
    bool changed = false;

    changed |= ImGui::DragFloatRange2("Range", &range[0], &range[1]);
    changed |= ImGui::InputFloat("Min", &range[0]);
    changed |= ImGui::InputFloat("Max", &range[1]);
    return changed;
}
