#pragma once


//based off of ihttps://github.com/alecjacobson/gptoolbox/blob/master/mex/eltopo.cpp


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#include <Eigen/Dense>
#pragma GCC diagnostic pop
#include <memory>

namespace LosTopos {
class SurfTrack;
class SubdivisionScheme;
struct SurfTrackInitializationParameters;
}// namespace LosTopos
namespace mtao::geometry::mesh {

class LosToposTracker {
  public:
    using EigenVec3d = Eigen::Vector3d;
    using EigenVec3i = Eigen::Vector3i;
    using ColVectors3d = Eigen::Matrix<double, 3, Eigen::Dynamic>;
    using ColVectors3i = Eigen::Matrix<int, 3, Eigen::Dynamic>;
    using RefCV3d = Eigen::Ref<ColVectors3d>;
    using RefCV3i = Eigen::Ref<ColVectors3i>;
    using CRefCV3d = Eigen::Ref<const ColVectors3d>;
    using CRefCV3i = Eigen::Ref<const ColVectors3i>;
    LosToposTracker &operator=(LosToposTracker &&);
    LosToposTracker &operator=(const LosToposTracker &);
    LosToposTracker(const LosToposTracker &);
    ~LosToposTracker();
    LosToposTracker(const CRefCV3d &V, const CRefCV3i &F, bool collision_safety = true, bool defrag_mesh = true, bool verbose = false);
    static Eigen::VectorXd dual_volumes(const CRefCV3d &V, const CRefCV3i &F);

    void split_edge(size_t edge_index);
    //split_triangle splits by the longest edge
    void split_triangle(size_t triangle_index);

    void initialize();
    void improve();
    double integrate(const CRefCV3d &Vnew, double dt);
    double step(const CRefCV3d &Vnew, double dt);

    void defrag_mesh();

    std::tuple<ColVectors3d, ColVectors3i> get_mesh() const;

    std::unique_ptr<LosTopos::SurfTrack> make_surftrack(const CRefCV3d &V, const CRefCV3i &F) const;

    std::unique_ptr<LosTopos::SurfTrackInitializationParameters> &init_params() { return m_init_params; }
    ColVectors3d get_vertices() const;
    ColVectors3i get_triangles() const;
    bool m_verbose = false;
    bool m_defrag_mesh = false;

  private:
    std::unique_ptr<LosTopos::SurfTrack> m_surf;
    std::shared_ptr<LosTopos::SubdivisionScheme> m_subdivision_scheme;
    std::unique_ptr<LosTopos::SurfTrackInitializationParameters> m_init_params;
    bool m_defrag_dirty = false;
};

}// namespace mtao::geometry::mesh
