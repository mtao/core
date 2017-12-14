#ifndef BOUNDARY_MATRIX_H
#define BOUNDARY_MATRIX_H

#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace mtao { namespace geometry { namespace mesh {

    template <typename T, typename FacetType>
auto boundary_matrix(const FacetType) const -> Eigen::SparseMatrix<T> {
    std::map<std::array<unsigned int,2>,int> edges;

    auto swap_and_sign = [](auto&& col) {
        T sign = 1;
        for(int i = 0; i < col.rows(); ++i) {
            auto&& vi = col(i);
            for(int j = i+1; j < col.rows(); ++j) {
                auto&& vj = col(j);
                if(vj < vi) {
                    sign *= -1;
                    std::swap(vi,vj);
                }
            }
        }
        return sign;
    };

    for(int i = 0; i < m_E.cols(); ++i) {
        auto e = m_E.col(i);
        edges.emplace(std::array<unsigned int,2>{{e(0),e(1)}},i);
    }

    std::vector<Eigen::Triplet<float>> trips;

    for(int i = 0; i < m_F.cols(); ++i) {
        for(int j = 0; j < 3; ++j) {
            auto jp = (j+1)%3;
            if(auto it = edges.find({m_F(j),m_F(jp)}); it != edges.end()) {
                trips.emplace_back(j,it->second,1);
            } else if(auto it = edges.find({m_F(jp),m_F(j)}); it != edges.end()) {
                trips.emplace_back(j,it->second,-1);
            }

        }
    }
    SparseMatrix A(m_F.cols(), m_E.cols());
    A.setFromTriplets(trips.begin(),trips.end());

    return A;
}


}}}


#endif//BOUNDARY_MATRIX_H
