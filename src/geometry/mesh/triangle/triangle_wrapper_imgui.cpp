#include "mtao/geometry/mesh/triangle/triangle_wrapper.h"
#include <imgui.h>
#include <sstream>
namespace mtao::geometry::mesh::triangle {
void triangle_opts::imgui_interface() {
    auto imgui_str = [](auto &&a, auto &&b) {
        std::stringstream ss;
        ss << b << "(" << a << ")";
        return ss.str();
    };
    auto setnum = [&](const std::string &title, auto &&v) {
        using VT = typename std::decay_t<decltype(v)>::value_type;

        bool enabled = bool(v);
        ImGui::Checkbox(title.c_str(), &enabled);
        if (enabled) {
            if (!v) {
                v = 0;
            }

            ImGui::SameLine();
            if constexpr (std::is_same_v<VT, int>) {
                ImGui::InputInt("", &(*(v)));
            } else if constexpr (std::is_same_v<VT, float>) {
                ImGui::InputFloat("", &(*(v)));
            }
        } else {
            v = std::nullopt;
        }
    };
#define BOOL_CASE(C, M) ImGui::Checkbox(imgui_str(C, #M).c_str(), &M);
#define NUM_VAL(C, M) setnum(imgui_str(C, #M), M);

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
    ImGui::InputInt(imgui_str("v", "verbosity").c_str(), &verbosity);
}
}// namespace mtao::geometry::mesh::triangle
