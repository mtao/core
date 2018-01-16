#include "mtao/geometry/strands.h"
#include <algorithm>
#include <iostream>
namespace mtao { namespace geometry {

    strand::strand(int a, int b): data({a,b}) {}
    std::optional<std::array<int,2>> strand::joint_ends(const strand& other) const {

        std::set<int> join;
        auto e0 = ends();
        auto e1 = other.ends();
        std::set_symmetric_difference(e0.begin(),e0.end(),e1.begin(),e1.end(),std::inserter(join,join.end()));
        if(join.size() == 2) {
            std::array<int,2> ends;
            std::copy(join.begin(),join.end(),ends.begin());
            return ends;
        } else {
            return {};
        }

    }
    void strand::merge(strand& other) {
        if(this == &other) {
            return;
        }
        if(data.empty()) {
            data = std::move(other.data);
            return;
        }
        if(other.data.empty()) {
            return;
        }
        auto jeo = joint_ends(other);
        if(!jeo) {
            return;
        }
        auto& je = *jeo;
        if(je[0] == front() || je[1] == back()) {
            data.reverse();
        }
        if(je[0] == other.front() || je[1] == other.back()) {
            other.data.reverse();
        }
        if(je[0] == front()) {
            data.splice(data.end(),other.data);
        } else {
            data.splice(data.begin(),other.data);
        }
        data.unique();
    }
    std::set<int> strand::ends() const {
        if(!data.empty()) {
            return {front(),back()};
        } else {
            return {};
        }
    }
}}
