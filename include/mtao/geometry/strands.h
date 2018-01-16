#pragma once
#include <list>
#include <set>
#include <optional>
#include <algorithm>

namespace mtao { namespace geometry {
struct strand {
    strand() = default;
    strand(int a, int b);
    std::list<int> data;

    int front() const { return data.front(); }
    int back() const { return data.back(); }

    std::set<int> ends() const;
    std::optional<std::array<int,2>> joint_ends(const strand& other) const;
    //assumes ther 
    void merge(strand& other);




};

template <typename Container>
strand merge(Container& c) {
    strand s;
    int starting_size = c.size();
    for(int j = 0; j < starting_size && !c.empty(); ++j) {
        for(auto&& f: c) {
            s.merge(f);
        }
        c.erase(std::remove_if(c.begin(),c.end(),[](auto&& f) {return f.data.empty();}));
    }
    return s;
}
}}
