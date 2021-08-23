#include "filter.hpp"
#include <set>
#include "mtao/visualization/imgui/utils.hpp"
#include "mtao/opengl/objects/sphere.hpp"
#include <mtao/json/vector.hpp>
#include <imgui.h>
#include <mtao/iterator/enumerate.hpp>
#include <spdlog/spdlog.h>

const std::array<std::string, int(Filter::FilterMode::END)> Filter::AllFilterModeNames = { "Current", "Interval", "All" };
auto Filter::filter_mode_preferences() const -> std::vector<FilterMode> {
    return { FilterMode::Current };
}
bool Filter::valid_filter_mode(FilterMode mode) const {
    for (auto &&mode2 : filter_mode_preferences()) {
        if (mode == mode2) {
            return true;
        }
    }
    return false;
}
auto Filter::filter_mode() const -> FilterMode {
    return _filter_mode;
}
void Filter::update_filter_mode(FilterMode mode) {
    if (valid_filter_mode(mode)) {
        _filter_mode = mode;
    }
}
bool Filter::filter_mode_gui() {
    static const auto all_items = mtao::visualization::imgui::utils::strs_to_charPtr(AllFilterModeNames);
    std::vector<const char *> items;
    auto modes = filter_mode_preferences();
    std::transform(modes.begin(), modes.end(), std::back_inserter(items), [&](FilterMode mode) -> const char * {
        return all_items[int(mode)];
    });


    int m = static_cast<char>(_filter_mode);
    if (ImGui::Combo("Filter Type", &m, items.data(), items.size())) {
        _filter_mode = static_cast<FilterMode>(char(m));
        return true;
    }
    return false;
}

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
auto Filter::particle_mask(const std::vector<Particles> &particles, int start, int end) const -> BoolVec {
    return particle_mask(particles.at(start));
}
std::vector<int> Filter::selected_particles(const std::vector<Particles> &particles, int start, int end) const {
    auto query = particle_mask(particles, start, end);
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


auto RangeFilter::filter_mode_preferences() const -> std::vector<FilterMode> {
    switch (_range_mode) {
        {
        default:
        case RangeMode::Velocity:
            return { FilterMode::Current };
        case Distance:
            return { FilterMode::Interval, FilterMode::All };
        }
    }
}
auto RangeFilter::particle_mask(const Particles &p) const -> BoolVec {

    if (filter_mode() != FilterMode::Current) {
        spdlog::warn("Filter mode must be Current in single-task particle mask");
    }
    if (_range_mode != RangeMode::Velocity) {
        spdlog::warn("Range Mode must be Velocity Particle for single-task particle mask");
    }
    const auto &V = p.velocities;
    auto vals = V.cast<float>().colwise().norm().transpose().eval();


    return (range[0] < vals.array()) && (vals.array() < range[1]);
}
auto RangeFilter::particle_mask(const std::vector<Particles> &particles, int start, int end) const -> BoolVec {


    switch (_range_mode) {
    default:
    case RangeMode::Velocity:
        return particle_mask(particles[start]);
    case RangeMode::Distance: {
        if (filter_mode() == FilterMode::All) {
            start = 0;
            end = particles.size();
        }
        mtao::VecXd d = mtao::VecXd::Zero(particles[start].count());
        for (int j = start; j < end - 1; ++j) {
            const auto &P = particles[j].positions;
            const auto &P2 = particles[j + 1].positions;
            d += (P - P2).colwise().norm().transpose();
            spdlog::info("did {} {}", j, d.norm());
        }
        return (range[0] < d.array()) && (d.array() < range[1]);
    }
    }
}
bool RangeFilter::gui() {
    bool changed = false;
    changed |= filter_mode_gui();
    {
        static const char *items[] = {
            "Distance",
            "Velocity",
        };
        int m = static_cast<char>(_range_mode);
        if (ImGui::Combo("Range Type", &m, items, IM_ARRAYSIZE(items))) {
            _range_mode = static_cast<RangeMode>(char(m));
            changed = true;
        }
    }

    changed |= ImGui::DragFloatRange2("Range", &range[0], &range[1]);
    changed |= ImGui::InputFloat("Min", &range[0]);
    changed |= ImGui::InputFloat("Max", &range[1]);
    return changed;
}

nlohmann::json Filter::config() const {

    nlohmann::json js;
    js["mode"] = AllFilterModeNames[int(_filter_mode)];
    return js;
}

void Filter::load_config(const nlohmann::json &js) {

    const std::string mode = js["mode"];
    for (auto &&[index, name] : mtao::iterator::enumerate(AllFilterModeNames)) {
        if (name == mode) {
            _filter_mode = FilterMode(index);
        }
    }
}
nlohmann::json PlaneFilter::config() const {

    nlohmann::json js = Filter::config();

    auto eqn = get_equation();
    js["equation"] = eqn;
    return js;
}

void PlaneFilter::load_config(const nlohmann::json &js) {
    Filter::load_config(js);
    auto eqn = js["equation"].get<mtao::Vec4f>();
    set_equation(eqn);
}
nlohmann::json SphereFilter::config() const {

    nlohmann::json js = Filter::config();
    js["radius"] = radius();
    js["subdivisions"] = subdivisions();
    js["center"] = center();
    return js;
}

void SphereFilter::load_config(const nlohmann::json &js) {
    Filter::load_config(js);
    set_radius(js["radius"].get<float>());
    set_subdivisions(js["subdivisions"].get<int>());
    set_center(js["center"].get<Eigen::Vector3f>());
}
nlohmann::json MeshFilter::config() const {

    nlohmann::json js = Filter::config();
    js["mesh_distance"] = mesh_distance;
    return js;
}

void MeshFilter::load_config(const nlohmann::json &js) {
    Filter::load_config(js);
    mesh_distance = js["mesh_distance"].get<float>();
}
nlohmann::json PruneFilter::config() const {

    nlohmann::json js = Filter::config();
    js["percentage"] = percentage;
    return js;
}

void PruneFilter::load_config(const nlohmann::json &js) {
    Filter::load_config(js);
    percentage = js["percentage"].get<float>();
}
nlohmann::json RangeFilter::config() const {

    nlohmann::json js = Filter::config();
    js["range"] = range;
    // TODO: range mode

    js["range_mode"] = _range_mode;
    return js;
}

void RangeFilter::load_config(const nlohmann::json &js) {
    Filter::load_config(js);
    range = js["range"].get<std::array<float, 2>>();
    _range_mode = js["range_mode"].get<RangeMode>();
    // TODO: range mode
}
nlohmann::json IntersectionFilter::config() const {

    nlohmann::json js = Filter::config();
    auto &arr = js["children"] = nlohmann::json::array({});
    for (auto &&[name, ptr, active] : filters) {
        arr.emplace_back(std::tuple{ name, active });
    }
    return js;
}

void IntersectionFilter::load_config(const nlohmann::json &js) {
    Filter::load_config(js);
    std::set<std::string> actives;
    for (auto &&[name, active] : js["children"].items()) {
        if (active) {
            actives.emplace(name);
        }
    }
    for (auto &&[name, ptr, active] : filters) {
        active = actives.contains(name);
    }
}


// auto JumpFilter::particle_mask(const Particles &p) const -> BoolVec {
//     throw std::invalid_argument("Cannot call single point particle mask on jump filter without more particles");
//     return BoolVec::Constant(p.size(), true);
// }
auto JumpFilter::particle_mask(const std::vector<Particles> &particles, int start, int end) const -> BoolVec {


    if (particles.size() == 1) {
        return BoolVec::Constant(particles.size(), true);
    }
    auto distances = [&](size_t j) -> mtao::VecXd {
        const auto &P = particles[j].positions;
        if (j == 0) {
            const auto &Q = particles[1].positions;
            return (Q - P).colwise().norm().transpose();
        } else if (j == particles.size() - 1) {
            const auto &Q = particles[j - 1].positions;
            return (P - Q).colwise().norm().transpose();
        } else {
            const auto &R = particles[j - 1].positions;
            const auto &Q = particles[j + 1].positions;
            return ((P - Q).colwise().norm() + (Q - R).colwise().norm()).transpose() / 2;
        }
    };

    auto estimate_timesteps = [&](int j) -> mtao::VecXd {
        auto d = distances(j);
        const auto &v = particles[j].velocities;
        return d.array() / v.colwise().norm().transpose().array();
    };

    mtao::VecXd timestep_estimates(particles.size());
    for (auto &&[j, dt] : mtao::iterator::enumerate(timestep_estimates)) {
        dt = estimate_timesteps(j).mean();
    }

    BoolVec R = BoolVec::Constant(true, particles[0].count());
    for (size_t j = 0; j < particles.size(); ++j) {
        size_t start = j - window;
        size_t end = j + window;
        if (window > j) {
            start = 0;
            end += window - j;
        } else if (size_t rev_j = particles.size() - j; window > rev_j) {
            size_t change = window - rev_j;
            if (change < start) {
                start -= window - (particles.size() - j);
            } else {
                start = 0;
            }
        }
        end = std::min(particles.size(), j);
        double mean_ts = timestep_estimates.segment(start, end - start).mean();
        auto ts = estimate_timesteps(j);
        R = R && (ts.array() < mean_ts);
    }


    //mtao::VecXf vec = Eigen::VectorXf::Random(p.count()).eval();
    //vec = (vec.array() + 1.f) / 2.f;
    //return vec.array() < percentage;
    return R;
}
bool JumpFilter::gui() {
    bool changed = false;
    changed |= ImGui::SliderFloat("Percentage", &percentage, 0.f, 1.f);
    return changed;
}

auto JumpFilter::filter_mode_preferences() const -> std::vector<FilterMode> {
    return { FilterMode::All };
}

nlohmann::json JumpFilter::config() const {

    nlohmann::json js = Filter::config();
    js["percentage"] = percentage;
    js["window"] = window;
    return js;
}

void JumpFilter::load_config(const nlohmann::json &js) {
    Filter::load_config(js);
    percentage = js["percentage"].get<float>();
    window = js["window"].get<size_t>();
}
