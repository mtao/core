#include <iostream>
#include "mtao/geometry/mesh/halfedge.hpp"
#include "mtao/geometry/mesh/halfedge_fv_map.hpp"

#include "mtao/logging/logger.hpp"
using namespace mtao::logging;

void test0() {
    mtao::ColVectors<float,2> V(2,4);
    mtao::ColVectors<int,2> E(2,V.cols() + V.cols() - 3);

    float dt = 2*M_PI / V.cols();
    for(int i = 0; i < V.cols(); ++i) {
        auto v = V.col(i);
        v(0) = std::cos(i*dt);
        v(1) = std::sin(i*dt);
        auto e = E.col(i);
        e(0) = i;
        e(1) = (i+1)%V.cols();

        

    }
    for(int i = 2; i < V.cols() - 1; ++i) {
        auto e = E.col(V.cols() + i - 2);
        e(0) = 0;
        e(1) = i;
        std::cout << "Cross: " << e.transpose() << std::endl;
    }
    std::cout << E << std::endl;

    using namespace mtao::geometry::mesh;
    auto hem = EmbeddedHalfEdgeMesh<float,2>::from_edges(V,E);

    std::cout << hem.edges() << std::endl << std::endl;



    hem.make_topology();
    std::cout << hem.edges() << std::endl << std::endl;

    auto C = hem.cells();
    for(size_t i = 0; i < C.size(); ++i) {
        std::cout << hem.edge(C[i]).cell() << ": ";
        cell_iterator(&hem,C[i])([&](auto&& e) {
                std::cout << e.vertex() << " ";
                });
        std::cout << std::endl;
        std::cout << std::endl << std::endl;
    }
}

void test1() {
    mtao::ColVectors<float,2> V(2,5);
    mtao::ColVectors<int,2> E(2,6);

    V.col(0) = mtao::Vec2f(0.,0.);
    V.col(1) = mtao::Vec2f(1.,0.);
    V.col(2) = mtao::Vec2f(1.,1.);
    V.col(3) = mtao::Vec2f(0.,1.);
    V.col(4) = mtao::Vec2f(.5f,.5f);

    E.col(0) = mtao::Vec2i(0,1);
    E.col(1) = mtao::Vec2i(1,2);
    E.col(2) = mtao::Vec2i(2,3);
    E.col(3) = mtao::Vec2i(3,0);
    E.col(4) = mtao::Vec2i(0,4);
    E.col(5) = mtao::Vec2i(4,1);
        

    std::cout << E << std::endl;

    using namespace mtao::geometry::mesh;
    auto hem = EmbeddedHalfEdgeMesh<float,2>::from_edges(V,E);

    std::cout << hem.edges() << std::endl << std::endl;



    hem.make_topology();
    std::cout << hem.edges() << std::endl << std::endl;

    auto C = hem.cells();
    for(size_t i = 0; i < C.size(); ++i) {
        std::cout << hem.edge(C[i]).cell() << ": ";
        cell_iterator(&hem,C[i])([&](auto&& e) {
                std::cout << e.vertex() << " ";
                });
        std::cout << std::endl;
        std::cout << std::endl << std::endl;
    }
}

void test2() {
    mtao::ColVectors<float,2> V(2,5);
    mtao::ColVectors<int,2> E(2,5);

    V.col(0) = mtao::Vec2f(0.,0.);
    V.col(1) = mtao::Vec2f(1.,0.);
    V.col(2) = mtao::Vec2f(1.,1.);
    V.col(3) = mtao::Vec2f(0.,1.);
    V.col(4) = mtao::Vec2f(.5f,.5f);

    E.col(0) = mtao::Vec2i(0,1);
    E.col(1) = mtao::Vec2i(1,2);
    E.col(2) = mtao::Vec2i(2,3);
    E.col(3) = mtao::Vec2i(3,0);
    E.col(4) = mtao::Vec2i(0,4);
        

    std::cout << E << std::endl;

    using namespace mtao::geometry::mesh;
    auto hem = EmbeddedHalfEdgeMesh<float,2>::from_edges(V,E);

    std::cout << hem.edges() << std::endl << std::endl;



    hem.make_topology();
    std::cout << hem.edges() << std::endl << std::endl;

    for(auto&& c: hem.cells()) {
        std::cout << hem.edge(c).cell() << ": ";
        cell_iterator(&hem,c)([&](auto&& e) {
                std::cout << e.vertex() << " ";
                });
        std::cout << std::endl;
        std::cout << std::endl << std::endl;
    }
}

int main(int argc, char * argv[]) {

    test0();
    std::cout << "===================================================" << std::endl;
    test1();
    std::cout << "===================================================" << std::endl;
    test2();
    


}

