#include <array>
#include "mtao/geometry/mesh/read_plc.hpp"
#include <fstream>
#include <iterator>
#include "mtao/logging/logger.hpp"

using namespace mtao::logging;
using namespace mtao::geometry::mesh::plc;

namespace mtao { namespace geometry { namespace mesh {



std::tuple<mtao::ColVectors<float,3>,MarkerType> read_nodeF(const std::string& filename) {return read_file<float,3>(filename);}
std::tuple<mtao::ColVectors<double,3>,MarkerType> read_nodeD(const std::string& filename) {return read_file<double,3>(filename);}
std::tuple<IndexType<2>, MarkerType> read_edge(const std::string& filename) {return read_file<int,2>(filename);}
std::tuple<IndexType<3>,MarkerType> read_face(const std::string& filename) {return read_file<int,3>(filename);}
std::tuple<IndexType<4>,MarkerType> read_ele(const std::string& filename) {return read_file<int,4>(filename);}
}}}
