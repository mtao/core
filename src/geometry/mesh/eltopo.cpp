#include <array>
#include <fstream>
#include <iterator>
#include <iostream>
#include "mtao/geometry/mesh/eltopo.h"
#include <eltopo3d/subdivisionscheme.h>
#include <eltopo3d/surftrack.h>


ElTopoTracker &ElTopoTracker::operator=(ElTopoTracker &&) = default;
ElTopoTracker &ElTopoTracker::operator=(const ElTopoTracker &o) {
    m_subdivision_scheme = o.m_subdivision_scheme;
    m_verbose = o.m_verbose;
    m_defrag_mesh = o.m_defrag_mesh;
    m_defrag_dirty = o.m_defrag_dirty;
    m_init_params = std::make_unique<SurfTrackInitializationParameters>(*o.m_init_params);
    m_init_params->m_subdivision_scheme = m_subdivision_scheme.get();
    auto [V, F] = o.get_mesh();
    m_surf = make_surftrack(V, F);
    return *this;
}
ElTopoTracker::ElTopoTracker(const ElTopoTracker &o) {
    m_subdivision_scheme = o.m_subdivision_scheme;
    m_verbose = o.m_verbose;
    m_defrag_mesh = o.m_defrag_mesh;
    m_defrag_dirty = o.m_defrag_dirty;
    m_init_params = std::make_unique<SurfTrackInitializationParameters>(*o.m_init_params);
    m_init_params->m_subdivision_scheme = m_subdivision_scheme.get();
    auto [V, F] = o.get_mesh();
    m_surf = make_surftrack(V, F);
}

ElTopoTracker::ElTopoTracker(const CRefCV3d &V, const CRefCV3i &F, bool collision_safety, bool defrag_mesh, bool verbose) : m_verbose(verbose), m_defrag_mesh(defrag_mesh), m_init_params(std::make_unique<SurfTrackInitializationParameters>()) {

    if (m_verbose) std::cout << "Starting constructor!" << std::endl;

    m_init_params->m_use_fraction = true;
    m_init_params->m_min_edge_length = .5;
    m_init_params->m_max_edge_length = 1.5;
    m_init_params->m_max_volume_change = .1;

    m_init_params->m_min_curvature_multiplier = 1.0;
    m_init_params->m_max_curvature_multiplier = 1.0;
    m_init_params->m_merge_proximity_epsilon = 0.001;
    m_init_params->m_proximity_epsilon = 1e-4;
    m_init_params->m_friction_coefficient = 0.0;
    m_init_params->m_perform_improvement = true;
    m_init_params->m_allow_topology_changes = false;
    m_init_params->m_allow_non_manifold = false;
    m_init_params->m_collision_safety = collision_safety;


    m_subdivision_scheme.reset(new ButterflyScheme());
    m_init_params->m_subdivision_scheme = m_subdivision_scheme.get();
    if (m_verbose) std::cout << "Made initial parameters" << std::endl;


    m_surf = make_surftrack(V, F);
    if (m_verbose) std::cout << "Defrag time!" << std::endl;

    if (m_defrag_mesh) this->defrag_mesh();

    if (m_verbose) std::cout << "Collision safety?" << std::endl;

    if (m_surf->m_collision_safety) {
        m_surf->m_collision_pipeline.assert_mesh_is_intersection_free(false);
    }
    if (m_verbose) std::cout << "Finished constructor!" << std::endl;
}
std::unique_ptr<SurfTrack> ElTopoTracker::make_surftrack(const CRefCV3d &V, const CRefCV3i &F) const {

    std::vector<Vec3st> tris(F.cols());
    std::vector<Vec3d> verts(V.cols());
    std::vector<double> masses(V.cols());

    for (size_t i = 0; i < verts.size(); ++i) {
        Eigen::Map<EigenVec3d> v(&verts[i][0]);
        v = V.col(i);
    }
    for (size_t i = 0; i < tris.size(); ++i) {
        Eigen::Map<Eigen::Matrix<size_t, 3, 1>> t(&tris[i][0]);
        t = F.col(i).cast<size_t>();
    }

    if (m_verbose) std::cout << "Making volumes!" << std::endl;

    auto dv = dual_volumes(V, F);
    Eigen::Map<Eigen::VectorXd>(masses.data(), masses.size()) = dv;

    if (m_verbose) std::cout << "Making surface!" << std::endl;

    return std::unique_ptr<SurfTrack>(new SurfTrack(verts, tris, masses, *m_init_params));
}

ElTopoTracker::~ElTopoTracker() {
}

auto ElTopoTracker::get_mesh() const -> std::tuple<ColVectors3d, ColVectors3i> {
    assert(m_surf);
    if (m_defrag_mesh && m_defrag_dirty) {

        const_cast<ElTopoTracker *>(this)->defrag_mesh();
    }
    return std::make_tuple(get_vertices(), get_triangles());
}
auto ElTopoTracker::get_vertices() const -> ColVectors3d {
    assert(m_surf);
    if (m_defrag_mesh && m_defrag_dirty) {
        const_cast<ElTopoTracker *>(this)->defrag_mesh();
    }
    auto &&pos = m_surf->get_positions();
    assert(pos.size() == m_surf->get_num_vertices());
    ColVectors3d V(3, pos.size());
    for (size_t i = 0; i < pos.size(); ++i) {
        V.col(i) = Eigen::Map<const EigenVec3d>(&pos[i][0]);
    }
    return V;
}
auto ElTopoTracker::get_triangles() const -> ColVectors3i {
    assert(m_surf);
    if (m_defrag_mesh && m_defrag_dirty) {

        const_cast<ElTopoTracker *>(this)->defrag_mesh();
    }
    auto &&tris = m_surf->m_mesh.get_triangles();
    ColVectors3i F(3, tris.size());
    for (size_t i = 0; i < tris.size(); ++i) {
        auto &t = tris[i];
        F.col(i) = Eigen::Map<const Eigen::Matrix<size_t, 3, 1>>(&t[0]).cast<int>();
    }
    assert(F.maxCoeff() < int(m_surf->get_num_vertices()));
    return F;
}

void ElTopoTracker::defrag_mesh() {
    assert(m_surf);
    m_surf->defrag_mesh();
}

void ElTopoTracker::improve() {
    assert(m_surf);
    // Improve
    m_surf->improve_mesh();
    // Topology changes
    m_surf->topology_changes();
    if (m_defrag_mesh) defrag_mesh();
}


double ElTopoTracker::step(const CRefCV3d &V, double dt) {
    double r = integrate(V, dt);
    improve();
    return r;
}
double ElTopoTracker::integrate(const CRefCV3d &V, double dt) {
    assert(m_surf);

    double ret_dt;
    std::vector<Vec3d> verts(V.cols());
    for (size_t i = 0; i < verts.size(); ++i) {
        Eigen::Map<EigenVec3d> v(&verts[i][0]);
        v = V.col(i);
    }

    m_surf->set_all_newpositions(verts);
    m_surf->integrate(dt, ret_dt);
    m_surf->set_positions_to_newpositions();
    return ret_dt;
}

void ElTopoTracker::split_edge(size_t edge_index) {
    assert(m_surf);
    auto &&splitter = m_surf->m_splitter;
    if (splitter.edge_is_splittable(edge_index)) {
        std::cout << "Splitting edge: " << edge_index << std::endl;
        splitter.split_edge(edge_index);
        m_defrag_dirty = true;
    }
}
void ElTopoTracker::split_triangle(size_t triangle_index) {
    auto &&edges = m_surf->m_mesh.m_triangle_to_edge_map[triangle_index];
    Vec3d lens;
    for (int i = 0; i < 3; ++i) {
        lens[i] = m_surf->get_edge_length(edges[i]);
    }
    int mc = 0;
    for (int i = 1; i < 3; ++i) {
        if (lens[i] > lens[mc]) {
            mc = i;
        }
    }

    split_edge(edges[mc]);
}

Eigen::VectorXd ElTopoTracker::dual_volumes(const CRefCV3d &V, const CRefCV3i &F) {

    Eigen::VectorXd Vols = Eigen::VectorXd::Zero(V.cols());

    auto double_vol = [&](int ai, int bi, int ci) -> double {
        auto a = V.col(ai);
        auto b = V.col(bi);
        auto c = V.col(ci);
        return (b - a).cross(c - a).norm();
    };
    for (int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        double v = double_vol(f(0), f(1), f(2));
        for (int j = 0; j < 3; ++j) {
            Vols(f(j)) += v;
        }
    }
    Vols /= 6;// a third for each cell, half for the double-vols
    return Vols;
}
