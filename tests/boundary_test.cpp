#include <iostream>

#include <mtao/geometry/mesh/boundary_facets.h>
#include <mtao/geometry/mesh/boundary_matrix.h>

int main() {
    {
        mtao::ColVectors<int,2> E(2,5);
        for(int i = 0; i < E.cols(); ++i) {
            E.col(i) = mtao::Vector<int,2>(i,i+1);
        }
        auto P = mtao::geometry::mesh::boundary_facets(E);
        std::cout << P << std::endl;
        auto B = mtao::geometry::mesh::boundary_matrix<float>(E,P);
        std::cout << B << std::endl << std::endl;
    }
    std::cout << "=====" << std::endl;
    {
        mtao::ColVectors<int,3> T(3,5);
        for(int i = 0; i < T.cols(); ++i) {
            T.col(i) = mtao::Vector<int,3>(T.cols(),i,(i+1)%T.cols());
        }

        auto E = mtao::geometry::mesh::boundary_facets(T);
        std::cout << E << std::endl << std::endl;;
        auto P = mtao::geometry::mesh::boundary_facets(E);
        std::cout << P << std::endl;
        auto B = mtao::geometry::mesh::boundary_matrix<float>(T,E);
        std::cout << B << std::endl << std::endl;
        auto B2 = mtao::geometry::mesh::boundary_matrix<float>(E,P);
        std::cout << B2 << std::endl << std::endl;
        std::cout << B2 << std::endl << std::endl;
        std::cout << B2*B << std::endl << std::endl;
    }
    std::cout << "=====" << std::endl;
    {
        mtao::ColVectors<int,4> T(4,5);
        for(int i = 0; i < T.cols(); ++i) {
            T.col(i) = mtao::Vector<int,4>(T.cols(),T.cols()+1,i%(T.cols()),(i+1)%(T.cols()));
        }

        std::cout << T << std::endl;
        auto F = mtao::geometry::mesh::boundary_facets(T);
        std::cout << F << std::endl;
        auto E = mtao::geometry::mesh::boundary_facets(F);
        std::cout << E << std::endl;
        auto B = mtao::geometry::mesh::boundary_matrix<float>(T,F);
        auto B2 = mtao::geometry::mesh::boundary_matrix<float>(F,E);
        std::cout << B << std::endl << std::endl;
        std::cout << B2 << std::endl << std::endl;
        std::cout << B2*B << std::endl << std::endl;
    }
}
