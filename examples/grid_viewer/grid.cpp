#include "grid.h"
#include <array>
#include "mtao/geometry/grid/indexers/ordered_indexer.hpp"


using namespace mtao::geometry::grid::indexing;
mtao::ColVectors<float,3> make_vertices(const int i, const int j, const int k) {
    mtao::ColVectors<float,3> V(3,i*j*k);
    OrderedIndexer<3> idx({{i,j,k}});
    for(int ii = 0; ii < i; ++ii) {
        for(int jj = 0; jj < j; ++jj) {
            for(int kk = 0; kk < k; ++kk) {
                V.col(idx(ii,jj,kk)) = mtao::Vector<float,3>(static_cast<float>(ii)/(i-1),static_cast<float>(jj)/(j-1),static_cast<float>(kk)/(k-1));
            }
        }
    }
    return V;
}
mtao::ColVectors<unsigned int,2> make_edge_topology(const int i, const int j, const int k) {
    mtao::ColVectors<unsigned int,2> E(2,
            (i-1)*(j)*(k)
           +(i)*(j-1)*(k)
           +(i)*(j)*(k-1)
            );
    OrderedIndexer<3> idx({{i,j,k}});
    int counter = 0;
    for(int ii = 0; ii < i-1; ++ii) {
        for(int jj = 0; jj < j; ++jj) {
            for(int kk = 0; kk < k; ++kk) {
                unsigned int o = idx(ii,jj,kk);
                unsigned int x = idx(ii+1,jj,kk);
                E.col(counter++) = mtao::Vector<unsigned int,2>(o,x);
            }
        }
    }
    for(int ii = 0; ii < i; ++ii) {
        for(int jj = 0; jj < j-1; ++jj) {
            for(int kk = 0; kk < k; ++kk) {
                unsigned int o = idx(ii,jj,kk);
                unsigned int y = idx(ii,jj+1,kk);
                E.col(counter++) = mtao::Vector<unsigned int,2>(o,y);
            }
        }
    }
    for(int ii = 0; ii < i; ++ii) {
        for(int jj = 0; jj < j; ++jj) {
            for(int kk = 0; kk < k-1; ++kk) {
                unsigned int o = idx(ii,jj,kk);
                unsigned int z = idx(ii,jj,kk+1);
                E.col(counter++) = mtao::Vector<unsigned int,2>(o,z);
            }
        }
    }
    return E;
}
mtao::ColVectors<unsigned int,3> make_topology(const int i, const int j, const int k) {
    mtao::ColVectors<unsigned int,3> F(3,
            2*((i)*(j-1)*(k-1)
            +(i-1)*(j)*(k-1)
            +(i-1)*(j-1)*(k))
            );
    OrderedIndexer<3> idx({{i,j,k}});
    int counter = 0;
    for(int ii = 0; ii < i; ++ii) {
        for(int jj = 0; jj < j-1; ++jj) {
            for(int kk = 0; kk < k-1; ++kk) {
                std::array<int,4> x{{idx(ii,jj,kk),idx(ii,jj+1,kk),idx(ii,jj,kk+1),idx(ii,jj+1,kk+1)}};
                F.col(counter++) = mtao::Vector<unsigned int,3>(x[0],x[1],x[2]);
                F.col(counter++) = mtao::Vector<unsigned int,3>(x[1],x[2],x[3]);
            }
        }
    }
    for(int ii = 0; ii < i-1; ++ii) {
        for(int jj = 0; jj < j; ++jj) {
            for(int kk = 0; kk < k-1; ++kk) {
                std::array<int,4> x{{idx(ii,jj,kk),idx(ii+1,jj,kk),idx(ii,jj,kk+1),idx(ii+1,jj,kk+1)}};
                F.col(counter++) = mtao::Vector<unsigned int,3>(x[0],x[1],x[2]);
                F.col(counter++) = mtao::Vector<unsigned int,3>(x[1],x[2],x[3]);
            }
        }
    }
    for(int ii = 0; ii < i-1; ++ii) {
        for(int jj = 0; jj < j-1; ++jj) {
            for(int kk = 0; kk < k; ++kk) {
                std::array<int,4> x{{idx(ii,jj,kk),idx(ii+1,jj,kk),idx(ii,jj+1,kk),idx(ii+1,jj+1,kk)}};
                F.col(counter++) = mtao::Vector<unsigned int,3>(x[0],x[1],x[2]);
                F.col(counter++) = mtao::Vector<unsigned int,3>(x[1],x[2],x[3]);
            }
        }
    }
    return F;
}
std::tuple<mtao::ColVectors<float,3>,mtao::ColVectors<unsigned int,3>> make_mesh(const int i, const int j, const int k) {
    return {make_vertices(i,j,k),make_topology(i,j,k)};
}
