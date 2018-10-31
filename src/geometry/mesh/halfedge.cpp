#include "mtao/geometry/mesh/halfedge.hpp"
#include "mtao/data_structures/disjoint_set.hpp"

#include <map>
#include <tuple>
#include <set>
#include <algorithm>
#include <iterator>
#include "mtao/logging/logger.hpp"
#include <mtao/iterator/enumerate.hpp>
using namespace mtao::logging;

namespace mtao { namespace geometry { namespace mesh {
using HalfEdge = HalfEdgeMesh::HalfEdge;

HalfEdgeMesh::HalfEdgeMesh(const std::string& str) {
}

HalfEdgeMesh::HalfEdgeMesh(const Cells& F) {
    construct(F);
}
HalfEdgeMesh::HalfEdgeMesh(const Edges& E): m_edges(E) {}

HalfEdgeMesh::HalfEdgeMesh(const mtao::ColVectors<int,2>& E) {
    clear(2*E.cols());

    auto vi = vertex_indices();
    auto di = dual_indices();
    for(int i = 0; i < E.cols(); ++i) {
        auto e = E.col(i);
        vi(2*i) = e(0);
        vi(2*i+1) = e(1);
        di(2*i) = 2*i+1;
        di(2*i+1) = 2*i;
    }
}


void HalfEdgeMesh::clear(size_t new_size) {
    m_edges = Edges::Constant(int(Index::IndexEnd),int(new_size),-1);
}
void HalfEdgeMesh::construct(const Cells& F) {
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

    clear(size);

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
void HalfEdgeMesh::make_cells() {
    auto ci = cell_indices();
    mtao::data_structures::DisjointSet<int> ds;
    for(int i = 0; i < size(); ++i) {
        ci(i) = i;
        ds.add_node(i);
    }
    for(int i = 0; i < size(); ++i) {
        ds.join(i,next_index(i));
    }
    ds.reduce_all();

    std::map<int,int> reindexer;
    for(auto&& i: ds.root_indices()) {
        reindexer[ds.node(i).data] = reindexer.size();
    }

    for(int i = 0; i < size(); ++i) {
        ci(i) = reindexer[ds.get_root(i).data];
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

std::vector<int> HalfEdgeMesh::vertices() const {
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

HalfEdge HalfEdgeMesh::cell_edge(int idx) const {
    for(int i = 0; i < size(); ++i) {
        if(cell_index(i) == idx) {
            return edge(i);
        }
    }
    return HalfEdge(this);
}

HalfEdge HalfEdgeMesh::vertex_edge(int idx) const {
    int ret = -1;
    int dual = 0;
    for(int i = 0; i < size(); ++i) {
        auto vi = vertex_index(i);
        if(vi == idx) {
            if( dual != -1) {
                ret = i;
                dual = dual_index(ret);
                if(dual == -1) {
                    trace() << "Finding edge for vertex " <<  idx<< " and found a boundary"<<ret<<"!";
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


std::map<int,std::set<int>> HalfEdgeMesh::vertex_edges_no_topology() const {
    std::map<int,std::set<int>> map;
    for(int i = 0; i < size(); ++i) {
        map[vertex_index(i)].insert(i);
    }


    return map;
}



void cell_iterator::increment(HalfEdge& he) {
    he.next();
}
void vertex_iterator::increment(HalfEdge& he) {
    he.next().dual();
//    he.dual().next();
}
void boundary_iterator::increment(HalfEdge& he) {
    //Test that this works!
    if(he.dual() != -1) {
        return;
    }
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


bool HalfEdgeMesh::is_boundary(int index) const {
    return dual_index(index) == -1;
}

bool HalfEdgeMesh::is_boundary_vertex(int index) const {
    bool has_boundary_edge = false;

    vertex_iterator(vertex_edge(index)).run_earlyout([this,&has_boundary_edge](const HalfEdge& e) {
            if(is_boundary(e)) {
            has_boundary_edge = true;
            return false;
            }
            return true;
            });
    return !has_boundary_edge;

}
bool HalfEdgeMesh::is_boundary_cell(int index) const {
    bool has_boundary_edge = false;

    cell_iterator(cell_edge(index)).run_earlyout([this,&has_boundary_edge](const HalfEdge& e) {
            if(is_boundary(e)) {
            has_boundary_edge = true;
            return false;
            }
            return true;
            });
    return !has_boundary_edge;

}


int HalfEdgeMesh::num_cells() const {
    std::set<int> cells;
    auto e = cell_indices();
    for(int i = 0; i < size(); ++i) {
        cells.insert(e(i));
    }
    return cells.size();
}
int HalfEdgeMesh::num_vertices() const {
    std::set<int> vertexs;
    auto e = vertex_indices();
    for(int i = 0; i < size(); ++i) {
        vertexs.insert(e(i));
    }
    return vertexs.size();
}


std::vector<int> HalfEdgeMesh::cell(int i) const {
    std::vector<int> ret;
    cell_iterator(cell_edge(i))([&ret](const HalfEdge& e) {
            ret.push_back(e.vertex());
            });
    return ret;
}
std::vector<int> HalfEdgeMesh::dual_cell(int i) const {
    std::vector<int> ret;

    auto ve = vertex_edge(i);
    vertex_iterator iter(ve);
    //if(ve.dual_index() == -1) { return ret; }
    iter([&ret](const HalfEdge& e) {
            ret.push_back(e.cell());
            });
    return ret;
}
HalfEdgeMesh HalfEdgeMesh::submesh_from_cells(const std::set<int>& cell_indices) const {
    std::set<int> edge_indices;
    for(int i = 0; i < size(); ++i) {
        if(auto it = cell_indices.find(cell_index(i)); it != cell_indices.end()) {
            edge_indices.insert(i);
        }
    }
    return submesh_from_edges(edge_indices);
}
HalfEdgeMesh HalfEdgeMesh::submesh_from_edges(const std::set<int>& edge_indices) const {
    Edges new_edges(int(Index::IndexEnd),edge_indices.size());
    std::map<int,int> edge_reindexer;
    edge_reindexer[-1] = -1;
    for(auto [a,b]: mtao::iterator::enumerate(edge_indices)) {
        edge_reindexer[b] = a;
        new_edges.col(a) = edges().col(b);
        if(auto it = edge_indices.find(b); it == edge_indices.end()) {
            new_edges.col(a)(int(Index::DualIndex)) = -1;
        }
    }
    auto reindex = [&](int& idx) {
            idx = edge_reindexer[idx];
    };
    for(int i = 0; i < new_edges.cols(); ++i) {
            reindex(new_edges.col(i)(int(Index::DualIndex)));
            reindex(new_edges.col(i)(int(Index::NextIndex)));
    }
    return HalfEdgeMesh(new_edges);
}

}}}
