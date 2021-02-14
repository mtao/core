
#include "mtao/geometry/mesh/polygon_boundary_triangulation.hpp"
#include "mtao/geometry/mesh/triangle_fan.hpp"
#include "mtao/geometry/mesh/triangle/triangle_wrapper.h"
#include <stdexcept>


namespace mtao::geometry::mesh {

mtao::ColVecs3i triangulate_fan(const PolygonBoundaryIndices &PBI) {
    if (!PBI.is_simple()) {
        throw std::invalid_argument("Cannot compute a triangle fan with holes");
    }
    return triangle_fan(PBI.outer_loop());
}

// NOTE: assumes that main loop is CCW and the holes are CW
mtao::ColVecs3i triangulate_triangle(const PolygonBoundaryIndices &PBI, const mtao::ColVecs2d &V, std::string_view opts) {
    auto [P, F] = triangulate_triangle_with_steiner(PBI, V, opts);
    if (P.size() > 0) {
        throw std::runtime_error("Triangle produced steiner points when we didn't expect them");
    }
    return F;
}

// NOTE: assumes that main loop is CCW and the holes are CW
std::tuple<mtao::ColVecs2d, mtao::ColVecs3i> triangulate_triangle_with_steiner(const PolygonBoundaryIndices &PBI, const mtao::ColVecs2d &V, std::string_view opts) {
    //collect the number of edges
    int size = PBI.outer_loop_size();
    for (auto &&v : PBI.holes) {
        size += v.size();
    }


    //check the total number of vertices
    std::set<int> used_vertices{ PBI.outer_loop().begin(), PBI.outer_loop().end() };
    for (auto &&v : PBI.holes) {
        for (auto &&c : v) {
            used_vertices.insert(c);
        }
    }
    if (used_vertices.size() == 0) {
        return {};
    }

    //compactify the vertices
    mtao::map<int, int> reindexer;
    mtao::map<int, int> unreindexer;
    mtao::ColVecs2d newV(2, used_vertices.size());
    for (auto &&[i, v] : mtao::iterator::enumerate(used_vertices)) {
        reindexer[v] = i;
        newV.col(i) = V.col(v);
        unreindexer[i] = v;
    }

    //compactify the edges
    mtao::ColVecs2i E(2, size);
    mtao::vector<mtao::Vec2d> holes;
    size = 0;
    {
        auto run = [&](auto &&curve) {
            for (size_t i = 0; i < curve.size(); ++i) {
                auto e = E.col(size++);
                e(0) = reindexer[curve[i]];
                e(1) = reindexer[curve[(i + 1) % curve.size()]];
            }
        };
        run(PBI.outer_loop());
        for (auto &&h : PBI.holes) {
            run(h);
        }
    }
    mtao::geometry::mesh::triangle::Mesh m(newV, E);

    //set some arbitrary attributes?
    m.fill_attributes();
    for (int i = 0; i < m.EA.cols(); ++i) { m.EA(i) = i; }
    for (int i = 0; i < m.VA.cols(); ++i) { m.VA(i) = i; }
    bool points_added = false;
    mtao::ColVecs2d newV2;
    mtao::ColVecs3i newF;

    auto nm = mtao::geometry::mesh::triangle::triangle_wrapper(m, opts);

    if (nm.V.cols() > newV.cols()) {
        newV2 = nm.V;
        points_added = true;
    }

    newF = nm.F;
    for (int i = 0; i < newF.size(); ++i) {
        auto &&f = newF(i);
        if (unreindexer.find(f) == unreindexer.end()) {
            points_added = true;
            break;
        }
    }
    if (!points_added) {
        for (int i = 0; i < newF.size(); ++i) {
            auto &&f = newF(i);
            f = unreindexer[f];
        }
    }


    std::set<int> interior;
    for (int i = 0; i < newF.cols(); ++i) {
        mtao::Vec2d B = mtao::Vec2d::Zero();
        auto f = newF.col(i);
        for (int j = 0; j < 3; ++j) {
            if (points_added) {
                B += newV2.col(f(j));
            } else {
                B += V.col(f(j));
            }
        }
        B /= 3;
        double wn = 0;
        for (auto &&c : holes) {
            double mywn = mtao::geometry::winding_number(V, c, B);
            wn += mywn;
        }
        //std::cout << wn << " ";
        if (std::abs(wn) > .5) {
            interior.insert(i);
        }
    }
    if (interior.size() == 0) {
        for (int i = 0; i < newF.cols(); ++i) {
            interior.insert(i);
        }
    }
    mtao::ColVecs3i FF(3, interior.size());
    for (auto &&[i, b] : mtao::iterator::enumerate(interior)) {
        FF.col(i) = newF.col(b);
    }
    return { newV2, FF };
}


}// namespace mtao::geometry::mesh
