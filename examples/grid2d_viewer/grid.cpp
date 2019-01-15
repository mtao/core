#include "grid.h"
#include <array>


mtao::ColVectors<float,2> make_vertices(const int i, const int j) {
    mtao::ColVectors<float,2> V(2,i*j);
    auto idx = [i,j](int ii, int jj) { return index(i,j,ii,jj); };
    for(int ii = 0; ii < i; ++ii) {
        for(int jj = 0; jj < j; ++jj) {
            V.col(idx(ii,jj)) = mtao::Vector<float,2>(static_cast<float>(ii)/(i-1),static_cast<float>(jj)/(j-1));
        }
    }
    return V;
}
mtao::ColVectors<unsigned int,3> make_topology(const int i, const int j) {
    mtao::ColVectors<unsigned int,3> F(3,2*(i-1)*(j-1));
    auto idx = [i,j](int ii, int jj) { return index(i,j,ii,jj); };
    int counter = 0;
    for(int ii = 0; ii < i-1; ++ii) {
        for(int jj = 0; jj < j-1; ++jj) {
            std::array<int,4> x{{idx(ii,jj),idx(ii+1,jj),idx(ii,jj+1),idx(ii+1,jj+1)}};
            F.col(counter++) = mtao::Vector<unsigned int,3>(x[0],x[1],x[2]);
            F.col(counter++) = mtao::Vector<unsigned int,3>(x[1],x[2],x[3]);
        }
    }
    return F;
}
std::tuple<mtao::ColVectors<float,2>,mtao::ColVectors<unsigned int,3>> make_mesh(const int i, const int j) {
    return {make_vertices(i,j),make_topology(i,j)};
}
