#pragma once
#include "mesh.h"


namespace mtao::geometry::mesh::triangle {

struct triangle_opts {
    operator std::string() const;
    triangle_opts() = default;
    triangle_opts(const std::string_view &sv);
    triangle_opts(const char *c) : triangle_opts(std::string_view(c)) {}
    void imgui_interface();
    void parse_options(const std::string_view &sv);
    void clear();

    //-p Triangulates a Planar Straight Line Graph (.poly file).
    bool input_POLY = false;
    //-r Refines a previously generated mesh.
    bool refine = false;
    //-q Quality mesh generation with no angles smaller than 20 degrees. An alternate minimum angle may be specified after the `q'.
    std::optional<float> quality;
    //-a Imposes a maximum triangle area constraint. A fixed area constraint (that applies to every triangle) may be specified after the `a', or varying area constraints may be read from a .poly file or .area file.
    std::optional<float> area;
    //-u Imposes a user-defined constraint on triangle size.
    std::optional<float> size;
    //-A Assigns a regional attribute to each triangle that identifies what segment-bounded region it belongs to.
    bool attribute = false;
    //-c Encloses the convex hull with segments.
    bool convex_hull = false;
    //-D Conforming Delaunay: use this switch if you want all triangles in the mesh to be Delaunay, and not just constrained Delaunay; or if you want to ensure that all Voronoi vertices lie within the triangulation.
    bool conforming = false;
    //-j Jettisons vertices that are not part of the final triangulation from the output .node file (including duplicate input vertices and vertices ``eaten'' by holes).
    bool jettison = false;
    //-e Outputs (to an .edge file) a list of edges of the triangulation.
    bool output_EDGE = false;
    //-v Outputs the Voronoi diagram associated with the triangulation. Does not attempt to detect degeneracies, so some Voronoi vertices may be duplicated.
    bool voronoi = false;
    //-n Outputs (to a .neigh file) a list of triangles neighboring each triangle.
    bool output_NEIGH;
    //-g Outputs the mesh to an Object File Format (.off) file, suitable for viewing with the Geometry Center's Geomview package.
    bool output_OFF = false;
    //-B Suppresses boundary markers in the output .node, .poly, and .edge output files.
    bool suppress_boundary_markers = false;
    //-P Suppresses the output .poly file. Saves disk space, but you lose the ability to maintain constraining segments on later refinements of the mesh.
    bool no_output_POLY = true;
    //-N Suppresses the output .node file.
    bool no_output_NODE = true;
    //-E Suppresses the output .ele file.
    bool no_output_EDGE = true;
    //-I Suppresses mesh iteration numbers.
    bool no_output_iterations = true;
    //-O Suppresses holes: ignores the holes in the .poly file.
    bool no_output_holes = true;
    //-X Suppresses exact arithmetic.
    bool no_exact_arithmetic = false;
    //-z Numbers all items starting from zero (rather than one). Note that this switch is normally overrided by the value used to number the first vertex of the input .node or .poly file. However, this switch is useful when calling Triangle from another program.
    bool zero_indexing = true;
    //-o2 Generates second-order subparametric elements with six nodes each.
    std::optional<int> subparametric;
    //-Y Prohibits the insertion of Steiner points on the mesh boundary. If specified twice (-YY), it prohibits the insertion of Steiner points on any segment, including internal segments.
    bool no_steiner = false;
    //-S Specifies the maximum number of added Steiner points.
    std::optional<int> max_steiner;
    //-i Uses the incremental algorithm for Delaunay triangulation, rather than the divide-and-conquer algorithm.
    bool incremental = false;
    //-F Uses Steven Fortune's sweepline algorithm for Delaunay triangulation, rather than the divide-and-conquer algorithm.
    bool sweepline = false;
    //-l Uses only vertical cuts in the divide-and-conquer algorithm. By default, Triangle uses alternating vertical and horizontal cuts, which usually improve the speed except with vertex sets that are small or short and wide. This switch is primarily of theoretical interest.
    bool vertical_cuts = false;
    //-s Specifies that segments should be forced into the triangulation by recursively splitting them at their midpoints, rather than by generating a constrained Delaunay triangulation. Segment splitting is true to Ruppert's original algorithm, but can create needlessly small triangles. This switch is primarily of theoretical interest.
    bool midpoint_splitting = false;
    //-C Check the consistency of the final mesh. Uses exact arithmetic for checking, even if the -X switch is used. Useful if you suspect Triangle is buggy.
    bool consistency_check = false;
    //-Q Quiet: Suppresses all explanation of what Triangle is doing, unless an error occurs.
    bool quiet = true;
    //-V Verbose: Gives detailed information about what Triangle is doing. Add more `V's for increasing amount of detail. `-V' gives information on algorithmic progress and detailed statistics.
    int verbosity = 0;
    //-h Help: Displays complete instructions.
    bool help = false;
};
Mesh triangle_wrapper(const Mesh &m, const triangle_opts &opts = std::string_view("zPa.01qepcDQ"));
Mesh triangle_wrapper(const std::vector<Mesh> &components, const triangle_opts &opts = std::string_view("zPa.01qepcDQ"));
}// namespace mtao::geometry::mesh::triangle
