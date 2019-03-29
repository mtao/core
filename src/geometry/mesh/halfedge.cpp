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

HalfEdgeMesh HalfEdgeMesh::from_cells(const Cells& F, bool use_open_halfedges) {
    HalfEdgeMesh hem;
    if(use_open_halfedges) {
        hem.construct_open_halfedges(F);
    } else {
        hem.construct(F);
    }
    return hem;
}
HalfEdgeMesh::HalfEdgeMesh(const Edges& E): m_edges(E) {}

HalfEdgeMesh HalfEdgeMesh::from_edges(const mtao::ColVectors<int,2>& E) {
    HalfEdgeMesh hem;
    hem.clear(2*E.cols());

    auto vi = hem.vertex_indices();
    auto di = hem.dual_indices();
    for(int i = 0; i < E.cols(); ++i) {
        auto e = E.col(i);
        vi(2*i) = e(0);
        vi(2*i+1) = e(1);
        di(2*i) = 2*i+1;
        di(2*i+1) = 2*i;
    }
    return hem;
}
std::vector<std::vector<int>> HalfEdgeMesh::dual_cells() const {
    auto hes = cell_halfedges();
    std::vector<std::vector<int>> C(hes.size());
    std::transform(hes.begin(),hes.end(), C.begin(), [&](int he_in_cell) -> std::vector<int> {
            return dual_cell_he(he_in_cell);
            });
    return C;
}
std::map<int,std::vector<int>> HalfEdgeMesh::dual_cells_map() const {
    auto hes = cell_halfedges();
    std::map<int,std::vector<int>> C;
    for(int he_in_cell: hes) {
        C[vertex_index(he_in_cell)] = dual_cell_he(he_in_cell);
    }
    return C;
}
std::vector<std::vector<int>> HalfEdgeMesh::cells() const {
    auto hes = cell_halfedges();
    std::vector<std::vector<int>> C(hes.size());
    std::transform(hes.begin(),hes.end(), C.begin(), [&](int he_in_cell) -> std::vector<int> {
            return cell_he(he_in_cell);
            });
    return C;
}
std::map<int,std::vector<int>> HalfEdgeMesh::cells_map() const {
    auto hes = cell_halfedges();
    std::map<int,std::vector<int>> C;
    for(int he_in_cell: hes) {
        C[cell_index(he_in_cell)] = cell_he(he_in_cell);
    }
    return C;
}

std::vector<int> HalfEdgeMesh::cell(int i) const {
    return cell(cell_edge(i));
}
std::vector<int> HalfEdgeMesh::dual_cell(int i) const {
    return cell(vertex_edge(i));
}
std::vector<int> HalfEdgeMesh::one_ring(int i) const {
    return one_ring(vertex_edge(i));
}
std::vector<int> HalfEdgeMesh::cell_he(int he_in_cell) const {
    return cell(edge(he_in_cell));
}
std::vector<int> HalfEdgeMesh::dual_cell_he(int he_pointing_to_vertex) const {
    return dual_cell(edge(he_pointing_to_vertex));
}
std::vector<int> HalfEdgeMesh::one_ring_he(int he_pointing_to_vertex) const {
    return dual_cell(edge(he_pointing_to_vertex));
}
std::vector<int> HalfEdgeMesh::cell(const HalfEdge& e) const {
    std::vector<int> vertices;
    cell_iterator{e}([&vertices](const HalfEdge& e) {
            vertices.push_back(e.vertex());
            });
    return vertices;
}
std::vector<int> HalfEdgeMesh::dual_cell(const HalfEdge& e) const {
    std::vector<int> vertices;
    cell_iterator{e}([&vertices](const HalfEdge& e) {
            vertices.emplace_back(e.vertex());
            });
    return vertices;
}
std::vector<int> HalfEdgeMesh::one_ring(const HalfEdge& e) const {
    std::vector<int> vertices;
    vertex_iterator{e}([&vertices](const HalfEdge& e) {
            vertices.emplace_back(e.get_dual().vertex());
            });
    return vertices;
}


void HalfEdgeMesh::clear(size_t new_size) {
    m_edges = Edges::Constant(int(Index::IndexEnd),int(new_size),-1);
}
void HalfEdgeMesh::construct_open_halfedges(const Cells& F) {
    using Edge = std::tuple<int,int>;

    std::set<Edge> directed_edges;

    //Cell is the row until its first -1 entry
    auto cell_size = [](auto& f) -> int{
        for(int j = 0; j < f.rows(); ++j) {
            if(f(j) == -1) {
                return j;
            }
        }
        return f.rows();
    };

    auto add_both = [&](Edge e) {
        directed_edges.emplace(e);
        std::swap(std::get<0>(e),std::get<1>(e));
        directed_edges.emplace(e);
    };

    //total number of halfedges is the number of internal edges
    for(int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        int size = cell_size(f);
        if(size < 3) {
            continue;
        }
        Edge e{f(size-1),f(0)};
        add_both(e);
        for(int j = 0; j < f.rows(); ++j) {
            std::get<0>(e) = std::get<1>(e);
            if(f(j) == -1) {
                std::get<1>(e) = f(0);
                add_both(e);
                break;
            } else {
                std::get<1>(e) = f(j);
                add_both(e);
            }
        }
    }
    mtao::ColVectors<int,2> E(2,directed_edges.size()/2);
    {
        int counter = 0;
        for(auto&& [a,b]: directed_edges) {
            if(a < b) {
                auto e = E.col(counter++);
                e(0) = a; e(1) = b;
            }
        }
    }
    *this = from_edges(E);

    auto e2he = edge_to_halfedge();

    auto ci = cell_indices();
    auto ni = next_indices();
    std::set<std::array<int,2>> used_edges;
    for(int i = 0; i < F.cols(); ++i) {
        auto f = F.col(i);
        int size = cell_size(f);
        bool reverse = false;
        for(int j = 0; j < size; ++j) {
            if(used_edges.find({{f(j),f((j+1)%size)}}) != used_edges.end()) {
                reverse = true;
            }
        }
        std::array<int,2> terminal_edge{{f(size-1),f(0)}};
        if(reverse) {
            std::swap(terminal_edge[0],terminal_edge[1]);
        }
        int last = e2he.at(terminal_edge);
        int last_for_later = last;
        used_edges.emplace(terminal_edge);
        ci(last) = i;
        for(int j = 0; j < size-1; ++j) {
            std::array<int,2> edge{{f(j),f(j+1)}};
            if(reverse) {
                std::swap(edge[0],edge[1]);
            }

            used_edges.emplace(edge);
            int he = e2he.at(edge);
            ci(he) = i;
            if(reverse) {
                ni(he) = last;
            } else {
                ni(last) = he;
            }

            last = he;

        }
        int he = last_for_later;
            if(reverse) {
                ni(he) = last;
            } else {
                ni(last) = he;
            }
    }


}
void HalfEdgeMesh::construct(const Cells& F) {
    using Edge = std::tuple<int,int>;

    //Cell is the row until its first -1 entry
    auto cell_size = [](auto& f) -> int{
        for(int j = 0; j < f.rows(); ++j) {
            if(f(j) == -1) {
                return j;
            }
        }
        return f.rows();
    };

    int size = 0;

    //total number of halfedges is the number of internal edges
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
        bool reverse = false;
        for(int j = 0; j < size-1; ++j) {
            if(ei_map.find({f(j),f(j+1)}) != ei_map.end()) {
                reverse = true;
            }
        }
        int counter_start = counter;
        for(int j = 0; j < size-1; ++j) {
            int a = f(j),b = f(j+1);
            if(reverse) {
                std::swap(a,b);
            }

            m_edges.col(counter) = DatVec(a,i,counter+1,-1);
            ei_map[{a,b}] = counter++;
        }
        int a = f(size-1),b = f(0);
        if(reverse) {
            std::swap(a,b);
        }
        m_edges.col(counter) = DatVec(a,i,counter_start,-1);
        ei_map[{a,b}] = counter++;
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
    auto ni = next_indices();
    mtao::data_structures::DisjointSet<int> ds;
    ds.add_node(-1);
    //write unique cell indices for each he
    for(int i = 0; i < size(); ++i) {
        ds.add_node(i);
        ci(i) = i;
    }
    //converge them according to their next pointers
    for(int i = 0; i < size(); ++i) {
        int nidx = ni(i);
        ds.join(i,nidx);
    }

    ds.reduce_all();
    int null_end = ds.get_root(-1).data;
    std::map<int,int> reindexer;
    for(auto&& i: ds.root_indices()) {
            reindexer[ds.node(i).data] = reindexer.size();
    }
    reindexer[null_end] = -1;

    for(int i = 0; i < size(); ++i) {
        int root = ds.get_root(i).data;
        ci(i) = reindexer[root];
    }
}

std::vector<int> HalfEdgeMesh::cell_halfedges() const {
    std::map<int,int> cell_edge;
    for(int i = 0; i < size(); ++i) {
        int index = cell_index(i);

        if(index >= 0) {
            cell_edge[index] = i;
        }
    }
    std::vector<int> ret;
    std::transform(cell_edge.begin(),cell_edge.end(), std::back_inserter(ret), [](const std::pair<const int, int>& fe) -> int {
            return fe.second;
            });
    return ret;
}

std::vector<int> HalfEdgeMesh::vertex_halfedges() const {
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
std::vector<int> HalfEdgeMesh::boundary_halfedges() const {
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
    auto ce =  cell_edges(idx);
    if(ce.empty()) {
        return HalfEdge(this);
    } else {
        return *ce.begin();
    }
}

HalfEdge HalfEdgeMesh::vertex_edge(int idx) const {
    auto ve =  vertex_edges(idx);
    if(ve.empty()) {
        return HalfEdge(this);
    } else {
        return *ve.begin();
    }
}
std::set<HalfEdge> HalfEdgeMesh::cell_edges(int idx) const {
    std::set<HalfEdge> he;
    for(int i = 0; i < size(); ++i) {
        if(cell_index(i) == idx) {
            he.emplace(this,i);
        }
    }
    return he;
}
std::set<HalfEdge> HalfEdgeMesh::vertex_edges(int idx) const {
    std::set<HalfEdge> he;
    int dual = 0;
    for(int i = 0; i < size(); ++i) {
        auto vi = vertex_index(i);
        if(vi == idx) {
            if( dual != -1) {
                dual = dual_index(i);
                if(dual == -1) {
                    trace() << "Finding edge for vertex " <<  idx<< " and found a boundary"<<i<<"!";
                }
                he.emplace(this,i);
            }
        }
    }
    return he;
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
    if(he.dual()) {
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
            if(is_boundary(int(e))) {
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
            if(is_boundary(int(e))) {
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


HalfEdgeMesh HalfEdgeMesh::submesh_from_vertices(const std::set<int>& vertex_indices) const {
    std::set<int> edge_indices;
    for(int i = 0; i < size(); ++i) {
        if(auto it = vertex_indices.find(vertex_index(i)); it != vertex_indices.end()) {
            edge_indices.insert(i);
        }
    }
    return submesh_from_edges(edge_indices);
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
std::map<std::array<int,2>,int> HalfEdgeMesh::edge_to_halfedge(bool use_next) const {
    using E = std::array<int,2>;
    std::map<E,int> ret;
    for(int i = 0; i < size(); ++i) {
        if(use_next) {
            int ni = next_index(i);
            E e{{vertex_index(i),vertex_index(ni)}};
            ret[e] = ni;
        } else {
            int di = dual_index(i);
            E e{{vertex_index(di),vertex_index(i)}};
            ret[e] = i;
        }
    }
    return ret;
}

void HalfEdgeMesh::complete_boundary_cells() {
    /*
    std::vector<int> duals;
    for(int i = 0; i < size(); ++i) {
        if(dual_index(i) == -1) {
            duals.push_back(i);
        }
    }
    int oldsize = size();
    //get the reverse arrows for each edge
    std::map<int,int> reverse_arrows;
    for(int i = 0; i < oldsize; ++i) {
        auto ni = next_index(i);
        if(ni == -1) continue;
        if(dual_index(ni) == -1) {
        reverse_arrows[ni] = i;
        }
    }

    m_edges.conservativeResize(m_edges.rows(),oldsize+duals.size());
    for(int i = 0; i < duals.size(); ++i) {
        int myind = i+oldsize;
        int dualind = duals[i];
        dual_indices()(dualind) = myind;
        dual_indices()(myind) = dualind;
        cell_indices()(dualind) = -1;


    }

    std::set<int> no_vertex_edges;
    for(int i = 0; i < duals.size(); ++i) {
        int myind = i+oldsize;
        no_vertex_edges.insert(myind);
    }
    while(!no_vertex_edges.empty()) {
        size_t s = no_vertex_edges.size();
        bool found = false;
        for(auto it = no_vertex_edges.begin(); it != no_vertex_edges.end();
                found?no_vertex_edges.erase(it++):++it
           ) {
            found = false;
            int myind = *it;
            int dualind = dual_index(myind);
            if(auto it = reverse_arrows.find(dualind); it != reverse_arrows.end()) {
                vertex_indices()(myind) = vertex_index(it->second);
                found = true;
            }
        }
        //no changes, maybe something is unrecoverable
        if( no_vertex_edges.size() == s) {
            break;
        }
    }
    */

}


cell_iterator HalfEdgeMesh::get_cell_iterator(const HalfEdge& he) const{
    return cell_iterator(he);
}
vertex_iterator HalfEdgeMesh::get_vertex_iterator(const HalfEdge& he) const{
    return vertex_iterator(he);
}
boundary_iterator HalfEdgeMesh::get_boundary_iterator(const HalfEdge& he) const{
    return boundary_iterator(he);
}
cell_iterator HalfEdgeMesh::get_cell_iterator(int he_idx) const{
    return cell_iterator(HalfEdge(this,he_idx));
}
vertex_iterator HalfEdgeMesh::get_vertex_iterator(int he_idx) const{
    return vertex_iterator(HalfEdge(this,he_idx));
}
boundary_iterator HalfEdgeMesh::get_boundary_iterator(int he_idx) const{
    return boundary_iterator(HalfEdge(this,he_idx));
}

bool HalfEdgeMesh::check_cell_validity() const {
    auto hes = cell_halfedges();

    bool valid = true;
    for(auto&& he: hes) {
        int cell = cell_index(he);
        get_cell_iterator(he).run_earlyout([&](const HalfEdge& e) {
                valid = e.cell() == cell;
                return valid;
                });
        if(!valid) {
            break;
        }
    }
    return valid;
}
bool HalfEdgeMesh::check_vertex_validity() const {
    auto hes = vertex_halfedges();

    bool valid = true;
    for(auto&& he: hes) {
        int vertex = vertex_index(he);
        get_vertex_iterator(he).run_earlyout([&](const HalfEdge& e) {
                valid = e.vertex() == vertex;
                return valid;
                });
        if(!valid) {
            break;
        }
    }
    return valid;
}
}}}

