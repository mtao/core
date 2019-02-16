#pragma once


#include <vector>
#include <map>
#include <optional>
#include <iostream>

namespace mtao { namespace data_structures {

template <typename Data>
struct DisjointSet{

    DisjointSet() {}
    void add_node(const Data& d) {
        data_map[d] = nodes.size();
        nodes.emplace_back(d);
    }
    struct Node {
        Node(const Data& d): data(d) {}
        std::optional<size_t> parent;
        Data data;
        bool root() const { return !bool(parent); }
    };


    void join(Data a, Data b) {
        join_idx(data_map[a],data_map[b]);
    }
    void join_idx(size_t a, size_t b) {
        reduce_idx(a);
        reduce_idx(b);
        size_t ra = get_root_idx(a);
        size_t rb = get_root_idx(b);
        if(ra != rb) {
        nodes[ra].parent = rb;
        }
    }
    size_t get_root_idx(size_t idx) const {
        for(;!nodes[idx].root(); idx=*nodes[idx].parent);
        return idx;
    }
    const Node& get_root(size_t idx) const {
        return node(get_root_idx(idx));
    }
    const Node& node(size_t idx) const {
        return nodes[idx];
    }
    void reduce_idx(size_t idx) {
        size_t r = get_root_idx(idx);
        while(!nodes[idx].root() ) {
            size_t oidx = idx;
            idx = *nodes[idx].parent;
            nodes[oidx].parent = r;

        }
    }
    std::vector<size_t> root_indices() const {
        std::vector<size_t> ret;
        for(size_t i = 0; i < nodes.size(); ++i) {
            if(nodes[i].root()) {
                ret.push_back(i);
            }
        }
        return ret;
    }
    void reduce_all() {
        for(size_t i = 0; i < nodes.size(); ++i) {
            reduce_idx(i);
        }
    }

    size_t index(const Node& n) const {
        return std::distance(&nodes.front(),&n);
    }


    std::vector<Node> nodes;
    //maps input data to 
    std::map<Data,size_t> data_map;

};
}}
