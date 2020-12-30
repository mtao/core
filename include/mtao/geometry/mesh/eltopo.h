#pragma once


//based off of ihttps://github.com/alecjacobson/gptoolbox/blob/master/mex/eltopo.cpp


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#include <Eigen/Dense>
#pragma GCC diagnostic pop
#include <memory>

class SurfTrack;
class SubdivisionScheme;
struct SurfTrackInitializationParameters;

class ElTopoTracker {
  public:
    using EigenVec3d = Eigen::Vector3d;
    using EigenVec3i = Eigen::Vector3i;
    using ColVectors3d = Eigen::Matrix<double, 3, Eigen::Dynamic>;
    using ColVectors3i = Eigen::Matrix<int, 3, Eigen::Dynamic>;
    using RefCV3d = Eigen::Ref<ColVectors3d>;
    using RefCV3i = Eigen::Ref<ColVectors3i>;
    using CRefCV3d = Eigen::Ref<const ColVectors3d>;
    using CRefCV3i = Eigen::Ref<const ColVectors3i>;
    ElTopoTracker &operator=(ElTopoTracker &&);
    ElTopoTracker &operator=(const ElTopoTracker &);
    ElTopoTracker(const ElTopoTracker &);
    ~ElTopoTracker();
    ElTopoTracker(const CRefCV3d &V, const CRefCV3i &F, bool collision_safety = true, bool defrag_mesh = true, bool verbose = false);
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

    std::unique_ptr<SurfTrack> make_surftrack(const CRefCV3d &V, const CRefCV3i &F) const;

    std::unique_ptr<SurfTrackInitializationParameters> &init_params() { return m_init_params; }
    ColVectors3d get_vertices() const;
    ColVectors3i get_triangles() const;
    bool m_verbose = false;
    bool m_defrag_mesh = false;

  private:
    std::unique_ptr<SurfTrack> m_surf;
    std::shared_ptr<SubdivisionScheme> m_subdivision_scheme;
    std::unique_ptr<SurfTrackInitializationParameters> m_init_params;
    bool m_defrag_dirty = false;
};
