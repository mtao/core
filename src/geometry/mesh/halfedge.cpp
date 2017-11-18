#include "geometry/mesh/halfedge.hpp"
#include <map>
#include <tuple>
#include "logging/logger.hpp"
using namespace mtao::logging;

namespace mtao { namespace geometry { namespace mesh {
using HalfEdge = HalfEdgeMesh::HalfEdge;

HalfEdgeMesh::HalfEdgeMesh(const std::string& str) {
}

HalfEdgeMesh::HalfEdgeMesh(const Cells& F) {
    using Edge = std::tuple<int,int>;

    auto cell_size = [](auto& f) -> int{
        for(int j = 0; j < f.rows(); ++j) {
            if(f(j) == -1) {
                return j;
            }
        }
        return f.rows();
    };

    int size = 0;

    for(int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        size += cell_size(f);
    }

    m_edges = Edges::Constant(int(Index::IndexEnd),size,-1);

    std::map<Edge,int> ei_map;

    int counter = 0;
    for(int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        int size = cell_size(f);
        int counter_start = counter;
        for(int j = 0; j < size-1; ++j) {
            m_edges.col(counter) = DatVec(f(j),i,counter+1,-1);
            ei_map[{f(j),f(j+1)}] = counter++;
        }
        m_edges.col(counter) = DatVec(f(size-1),i,counter_start,-1);
        ei_map[{f(size-1),f(0)}] = counter++;
    }

    for(int i = 0; i < this->size(); ++i) {

        int my_edge = vertex_index(i);
        int next_edge = vertex_index(next_index(i)) ;


        if(auto it = ei_map.find({next_edge, my_edge});
                it != ei_map.end()) {
            dual_indices()(i) = it->second;
        }
    }

}

std::vector<int> HalfEdgeMesh::cells() const {
    std::map<int,int> cell_edge;
    for(int i = 0; i < size(); ++i) {
        cell_edge[cell_index(i)] = i;
    }
    std::vector<int> ret;
    std::transform(cell_edge.begin(),cell_edge.end(), std::back_inserter(ret), [](const std::pair<const int, int>& fe) -> int {
            return fe.second;
            });
    return ret;
}

std::vector<int> HalfEdgeMesh::dual_cells() const {
    //TODO: make sure every vertex gets expressed!
    std::map<int,int> cell_edge;
    for(int i = 0; i < size(); ++i) {
        auto vi = vertex_index(i);
        if(auto it = cell_edge.find(vi); it != cell_edge.end()) {
            if(dual_index(it->second) != -1) {
                it->second = i;
            }
        } else {
            cell_edge[vertex_index(i)] = i;
        }
    }
    std::vector<int> ret;
    std::transform(cell_edge.begin(),cell_edge.end(), std::back_inserter(ret), [](const std::pair<const int, int>& fe) -> int {
            return fe.second;
            });
    return ret;
}
std::vector<int> HalfEdgeMesh::boundary() const {
    std::vector<int> ret;
    for(int i = 0; i < size(); ++i) {
        if(dual_index(i) == -1) {
            ret.push_back(i);
        }
    }
    return ret;
}

HalfEdge HalfEdgeMesh::edge(int i) const {
    return HalfEdge(this,i);
}

HalfEdge HalfEdgeMesh::cell(int idx) const {
    for(int i = 0; i < size(); ++i) {
        if(cell_index(i) == idx) {
            return edge(i);
        }
    }
    return HalfEdge(this);
}

HalfEdge HalfEdgeMesh::vertex(int idx) const {
    int ret = -1;
    int dual = 0;
    for(int i = 0; i < size(); ++i) {
        auto vi = vertex_index(i);
        if(vi == idx) {

            if( dual != -1) {
                ret = i;
                dual = dual_index(ret);
                debug() << dual;
                if(dual == -1) {
                    debug() << "Edge vertex found!";
                }
            }
        }
    }

    return HalfEdge(this,ret);
}

HalfEdge::HalfEdge(const MeshType* cc, int idx): m_cc(cc), m_index(idx) {}

HalfEdge& HalfEdge::next() {
    m_index = next_index();
    return *this;
}
HalfEdge& HalfEdge::dual() {
    m_index = dual_index();
    return *this;
}
int HalfEdge::next_index() const {
    return m_cc->next_index(index());
}
int HalfEdge::dual_index() const {
    return m_cc->dual_index(index());
}
HalfEdge HalfEdge::get_next() const {
    return HalfEdge(m_cc,next_index());
}
HalfEdge HalfEdge::get_dual() const {
    return HalfEdge(m_cc,dual_index());
}





void cell_iterator::increment(HalfEdge& he) {
    he.next();
}
void dual_cell_iterator::increment(HalfEdge& he) {
    he.next().dual();
//    he.dual().next();
}
void boundary_iterator::increment(HalfEdge& he) {
    //Test that this works!
    he.next();

    while(he.dual_index() != -1) {
        he.dual().next();
    }

}


int HalfEdgeMesh::boundary_size() const {
    return (dual_indices().array() == -1).count();
}


namespace detail {
    void invalid_edge_warning() {
        warn() << "Edge iteration ended in an invalid edge";
    }
}


}}}
