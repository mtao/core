#include <iostream>
#include <mtao/geometry/kdtree.hpp>
#include <mtao/geometry/prune_vertices.hpp>
#include <algorithm>

int main(int argc, char *argv[]) {
    mtao::vector<mtao::Vector<double, 1>> p;
    p.push_back(mtao::Vector<double, 1>(0.));
    p.push_back(mtao::Vector<double, 1>(.5));
    p.push_back(mtao::Vector<double, 1>(.25));
    p.push_back(mtao::Vector<double, 1>(.75));
    p.push_back(mtao::Vector<double, 1>(.125));
    p.push_back(mtao::Vector<double, 1>(.375));
    p.push_back(mtao::Vector<double, 1>(.625));
    p.push_back(mtao::Vector<double, 1>(.875));
    p.push_back(mtao::Vector<double, 1>(1.));
    size_t cursize = p.size();

    p.push_back(mtao::Vector<double, 1>(.5));
    p.push_back(mtao::Vector<double, 1>(.25));
    p.push_back(mtao::Vector<double, 1>(.75));
    p.resize(1000);

    std::generate(p.begin() + cursize, p.end(), []() { return (mtao::Vector<double, 1>::Random().array() / 2 + .5).eval(); });


    auto [p2, remap] = mtao::geometry::prune(p, .0625);
    for (auto &&v : p2) {
        std::cout << v.transpose() << std::endl;
    }
    for (auto &&[a, b] : remap) {
        std::cout << a << "=>" << b << std::endl;
    }
    std::cout << "Initial size: " << cursize << std::endl;
    std::cout << "Final size: " << p2.size() << std::endl;
}
