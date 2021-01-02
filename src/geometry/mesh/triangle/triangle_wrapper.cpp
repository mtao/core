#include "mtao/geometry/mesh/triangle/triangle_wrapper.h"
#include "mtao/geometry/mesh/triangle/mesh.h"
#include <mtao/logging/timer.hpp>
#include <utility>
#include <mtao/type_utils.h>
using namespace mtao::logging;
#ifdef SINGLE
using REAL = float;
#else /* not SINGLE */
using REAL = double;
#endif /* not SINGLE */
using VOID = double;

extern "C" {
#include "triangle.h"
}
#include <algorithm>

using namespace mtao::logging;


namespace mtao::geometry::mesh::triangle {
triangle_opts::triangle_opts(const std::string_view &sv) {
    parse_options(sv);
}


Mesh triangle_wrapper(const std::vector<Mesh> &components, const triangle_opts &opts) {
    Mesh m = combine(components);
    return triangle_wrapper(m, opts);
}
Mesh triangle_wrapper(const Mesh &m, const triangle_opts &opts) {

    auto t = timer("triangle", true);
    auto V2 = m.V;
    auto E2 = m.E;
    auto H2 = m.H;
    auto VA2 = m.VA;
    auto EA2 = m.EA;
    struct triangulateio in, out;
    in.numberofpointattributes = 0;
    in.pointlist = V2.data();
    in.numberofpoints = V2.cols();
    in.segmentlist = E2.data();
    in.numberofsegments = E2.cols();
    in.holelist = H2.data();
    in.numberofholes = H2.cols();
    in.pointmarkerlist = VA2.data();
    in.segmentmarkerlist = EA2.data();
    //in.numberofholes=0;
    in.numberofregions = 0;


    out.pointlist = NULL;
    out.trianglelist = NULL;
    out.triangleattributelist = NULL;
    out.edgelist = NULL;
    out.edgemarkerlist = NULL;
    out.pointmarkerlist = NULL;


    triangulate(const_cast<char *>(std::string(opts).c_str()), &in, &out, NULL);
    Mesh m2;
    m2.V.resize(2, out.numberofpoints);

    std::copy(out.pointlist, out.pointlist + m2.V.size(), m2.V.data());

    m2.F.resize(3, out.numberoftriangles);

    std::copy(out.trianglelist, out.trianglelist + m2.F.size(), m2.F.data());

    m2.E.resize(2, out.numberofedges);
    std::copy(out.edgelist, out.edgelist + m2.E.size(), m2.E.data());
    m2.VA.resize(out.numberofpoints);
    std::copy(out.pointmarkerlist, out.pointmarkerlist + m2.VA.size(), m2.VA.data());
    m2.EA.resize(out.numberofedges);
    std::copy(out.edgemarkerlist, out.edgemarkerlist + m2.EA.size(), m2.EA.data());


    free(out.trianglelist);
    free(out.triangleattributelist);
    free(out.pointlist);
    free(out.edgelist);
    free(out.pointmarkerlist);
    free(out.edgemarkerlist);

    m2.make_circumcenters();


    return m2;
}


template<typename T>
T readnum(std::string_view::const_iterator &it, const std::string_view::const_iterator &end) {

    auto start = it;
    while (((*it > 47) && (*it < 58)) || *it == 45 || *it == 46) {
        ++it;
    }
    std::string s(start, std::distance(start, it));


    if constexpr (std::is_same_v<T, int>) {
        return std::stoi(s);
    } else if constexpr (std::is_same_v<T, float>) {
        return std::stof(s);
    }
}
void triangle_opts::parse_options(const std::string_view &sv) {


    clear();
    std::cout << sv << std::endl;
    for (auto strit = sv.begin(); strit != sv.end(); ++strit) {
        switch (*strit) {

#define BOOL_CASE(C, M) \
    case C:             \
        M = true;       \
        continue;
#define NUM_VAL(C, M)                                            \
    case C:                                                      \
        M = readnum<decltype(M)::value_type>(++strit, sv.end()); \
        continue;
            BOOL_CASE('p', input_POLY)
            BOOL_CASE('r', refine)
            BOOL_CASE('A', attribute)
            BOOL_CASE('c', convex_hull)
            BOOL_CASE('D', conforming)
            BOOL_CASE('j', jettison)
            BOOL_CASE('e', output_EDGE)
            BOOL_CASE('v', voronoi)
            BOOL_CASE('n', output_NEIGH)
            BOOL_CASE('g', output_OFF)
            BOOL_CASE('B', suppress_boundary_markers)
            BOOL_CASE('P', no_output_POLY)
            BOOL_CASE('N', no_output_NODE)
            BOOL_CASE('E', no_output_EDGE)
            BOOL_CASE('I', no_output_iterations)
            BOOL_CASE('O', no_output_holes)
            BOOL_CASE('X', no_exact_arithmetic)
            BOOL_CASE('z', zero_indexing)
            BOOL_CASE('Y', no_steiner)
            BOOL_CASE('i', incremental)
            BOOL_CASE('F', sweepline)
            BOOL_CASE('l', vertical_cuts)
            BOOL_CASE('s', midpoint_splitting)
            BOOL_CASE('C', consistency_check)
            BOOL_CASE('Q', quiet)
            BOOL_CASE('h', help)
            NUM_VAL('q', quality)
            NUM_VAL('a', area)
            NUM_VAL('u', size)
            NUM_VAL('o', subparametric)
            NUM_VAL('S', max_steiner)
#undef NUM_VAL
#undef BOOL_CASE
        case 'V':
            verbosity++;
            continue;


        default:
            continue;
        }
    }
}

void triangle_opts::clear() {

#define BOOL_CASE(C, M) M = false;
#define NUM_VAL(C, M) M = std::nullopt;
    BOOL_CASE('p', input_POLY)
    BOOL_CASE('r', refine)
    BOOL_CASE('A', attribute)
    BOOL_CASE('c', convex_hull)
    BOOL_CASE('D', conforming)
    BOOL_CASE('j', jettison)
    BOOL_CASE('e', output_EDGE)
    BOOL_CASE('v', voronoi)
    BOOL_CASE('n', output_NEIGH)
    BOOL_CASE('g', output_OFF)
    BOOL_CASE('B', suppress_boundary_markers)
    BOOL_CASE('P', no_output_POLY)
    BOOL_CASE('N', no_output_NODE)
    BOOL_CASE('E', no_output_EDGE)
    BOOL_CASE('I', no_output_iterations)
    BOOL_CASE('O', no_output_holes)
    BOOL_CASE('X', no_exact_arithmetic)
    BOOL_CASE('z', zero_indexing)
    BOOL_CASE('Y', no_steiner)
    BOOL_CASE('i', incremental)
    BOOL_CASE('F', sweepline)
    BOOL_CASE('l', vertical_cuts)
    BOOL_CASE('s', midpoint_splitting)
    BOOL_CASE('C', consistency_check)
    BOOL_CASE('Q', quiet)
    BOOL_CASE('h', help)
    NUM_VAL('q', quality)
    NUM_VAL('a', area)
    NUM_VAL('u', size)
    NUM_VAL('o', subparametric)
    NUM_VAL('S', max_steiner)
#undef NUM_VAL
#undef BOOL_CASE
    verbosity = 0;
}

triangle_opts::operator std::string() const {
    std::stringstream ss;
#define BOOL_CASE(C, M) \
    if (M) { ss << C; }
#define NUM_VAL(C, M) \
    if (M) { ss << C << *M; }
    BOOL_CASE('p', input_POLY)
    BOOL_CASE('r', refine)
    BOOL_CASE('A', attribute)
    BOOL_CASE('c', convex_hull)
    BOOL_CASE('D', conforming)
    BOOL_CASE('j', jettison)
    BOOL_CASE('e', output_EDGE)
    BOOL_CASE('v', voronoi)
    BOOL_CASE('n', output_NEIGH)
    BOOL_CASE('g', output_OFF)
    BOOL_CASE('B', suppress_boundary_markers)
    BOOL_CASE('P', no_output_POLY)
    BOOL_CASE('N', no_output_NODE)
    BOOL_CASE('E', no_output_EDGE)
    BOOL_CASE('I', no_output_iterations)
    BOOL_CASE('O', no_output_holes)
    BOOL_CASE('X', no_exact_arithmetic)
    BOOL_CASE('z', zero_indexing)
    BOOL_CASE('Y', no_steiner)
    BOOL_CASE('i', incremental)
    BOOL_CASE('F', sweepline)
    BOOL_CASE('l', vertical_cuts)
    BOOL_CASE('s', midpoint_splitting)
    BOOL_CASE('C', consistency_check)
    BOOL_CASE('Q', quiet)
    BOOL_CASE('h', help)
    NUM_VAL('q', quality)
    NUM_VAL('a', area)
    NUM_VAL('u', size)
    NUM_VAL('o', subparametric)
    NUM_VAL('S', max_steiner)
#undef NUM_VAL
#undef BOOL_CASE
    for (int i = 0; i < verbosity; ++i) {
        ss << "V";
    }
    return ss.str();
}

}// namespace mtao::geometry::mesh::triangle
