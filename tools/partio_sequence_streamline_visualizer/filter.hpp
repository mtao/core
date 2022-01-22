#pragma once
#include "particles.hpp"
#include <nlohmann/json.hpp>
#include <mtao/opengl/objects/plane.hpp>
#include <mtao/opengl/objects/sphere.hpp>
#include <igl/fast_winding_number.h>
#include <igl/AABB.h>

struct Filter {
    using BoolVec = Eigen::Array<bool, Eigen::Dynamic, 1>;

    virtual BoolVec particle_mask(const Particles &p) const = 0;
    std::vector<int> selected_particles(const Particles &p) const;

    // by default it just checks whether the start satisfies the filter
    virtual BoolVec particle_mask(const std::vector<Particles> &particles, int start, int end) const;

    std::vector<int> selected_particles(const std::vector<Particles> &particles, int start, int end) const;
    virtual bool gui() { return false; }
    enum FilterMode : char { Current = 0,
                             Interval = 1,
                             All = 2,
                             END };
    const static std::array<std::string, int(FilterMode::END)> AllFilterModeNames;
    // by default only supports currnet
    virtual std::vector<FilterMode> filter_mode_preferences() const;
    bool valid_filter_mode(FilterMode) const;

    void update_filter_mode(FilterMode mode);
    virtual nlohmann::json config() const;
    virtual void load_config(const nlohmann::json &);

    bool filter_mode_gui();
    FilterMode filter_mode() const;

  private:
    FilterMode _filter_mode = FilterMode::Current;
};

struct PlaneFilter : public Filter
  , public mtao::opengl::objects::Plane {

    BoolVec particle_mask(const Particles &p) const override;
    bool gui() override;
    nlohmann::json config() const override;
    void load_config(const nlohmann::json &) override;
};

struct SphereFilter : public Filter
  , public mtao::opengl::objects::PositionedSphere {

    BoolVec particle_mask(const Particles &p) const override;
    bool gui() override;
    nlohmann::json config() const override;
    void load_config(const nlohmann::json &) override;
};

struct MeshFilter : public Filter {
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::AABB<Eigen::MatrixXf, 3> aabb;
    igl::FastWindingNumberBVH fwn_bvh;
    Eigen::AffineCompact3d point_transform = Eigen::AffineCompact3d::Identity();
    float mesh_distance = .1;
    bool include_all_interior = true;// useful for open meshes
    bool include_interior = true;
    bool include_exterior = true;
    bool invert_selection = false;
    MeshFilter(const Eigen::MatrixXf &V, const Eigen::MatrixXi &F);
    BoolVec particle_mask(const Particles &p) const override;
    bool gui() override;
    nlohmann::json config() const override;
    void load_config(const nlohmann::json &) override;
};

// prunes a random subset of particles
struct PruneFilter : public Filter {
    BoolVec particle_mask(const Particles &p) const override;
    bool gui() override;
    float percentage = .5f;
    nlohmann::json config() const override;
    void load_config(const nlohmann::json &) override;
};

// prunes velocities outside of a specified range
struct RangeFilter : public Filter {
    BoolVec particle_mask(const Particles &p) const override;
    BoolVec particle_mask(const std::vector<Particles> &particles, int start, int end) const override;
    bool gui() override;
    std::array<float, 2> range = std::array<float, 2>{ { 0.f, 1.f } };
    std::vector<FilterMode> filter_mode_preferences() const override;

    enum RangeMode : char { Distance,
                            Velocity };
    RangeMode _range_mode;
    nlohmann::json config() const override;
    void load_config(const nlohmann::json &) override;
};

// prunes particles that move drastically far away from their velocity, using a window to estimate timesteps
struct JumpFilter : public Filter {
    BoolVec particle_mask(const Particles &p) const override;
    BoolVec particle_mask(const std::vector<Particles> &particles, int start, int end) const override;
    bool gui() override;
    size_t window = 2;
    float percentage = .5f;

    std::vector<FilterMode> filter_mode_preferences() const override;
    nlohmann::json config() const override;
    void load_config(const nlohmann::json &) override;
};

NLOHMANN_JSON_SERIALIZE_ENUM(RangeFilter::RangeMode, { { RangeFilter::RangeMode::Distance, "distance" }, { RangeFilter::RangeMode::Velocity, "velocity" } })


struct IntersectionFilter : public Filter {
    std::vector<std::tuple<std::string, std::shared_ptr<Filter>, bool>> filters;
    BoolVec particle_mask(const Particles &p) const override;
    //BoolVec particle_mask(const std::vector<Particles> &particles, int start, int end) const override;
    bool gui() override;
    nlohmann::json config() const override;
    void load_config(const nlohmann::json &) override;
};
