#pragma once
#include <vector>

namespace mtao::geometry {
    // returns a vector of monomial integrals
    // \int_{0}^1 \int_{0}^{1-x} x^j y^k dy dx
    // = (j+1)! (k+2)! / (j+k+3)!
    // = 1 / ( N choose J ) for J = j+1, N = K+1 )
    // returns a linear vector [ M1 M2 M3 M4 ]
    // where |Mk| = k, Mk = [ x^k x^{k-1}y ... y^k ]
    template <typename T>
    std::vector<T> triangle_monomial_integrals(size_t max_dim) {
        // max dim + 1 dimensiosn will be used
        std::vector<T> ret;
        size_t max_integral_deg = max_dim + 3;
        ret.reserve(((max_integral_deg+1) * (max_integral_deg+2))/2);;
        std::vector<size_t> pascal_row;

        pascal_row.emplace_back(1);
        ret.emplace_back(1);
        for(size_t d = 1; d < max_integral_deg; ++d) {
            std::vector<size_t> new_pascal_row(pascal_row.size()+1);
            new_pascal_row.front() = 1;
            new_pascal_row.back() = 1;
            for(size_t j = 0; j < pascal_row.size()-1; ++j) {
                new_pascal_row[j+1] = pascal_row[j] + pascal_row[j+1];
            }

            //for(auto&& idx: new_pascal_row) {
            //    std::cout << idx << " ";
            //}
            //std::cout << std::endl;
            if(d >= 2) {
                // j + k + 3 == d
                // j in [0,d-3]
                // jp1 in [1,d-2]
                //std::cout << "Monomials of max deg " << d-2 << std::endl;
                for(size_t j = 0; j < d - 1; ++j) {
                    //std::cout << "x^" << j << " y^" << d-2-j << " => ";
                    //std::cout << (1. / new_pascal_row[j+1]) << std::endl;
                    ret.emplace_back(1. / new_pascal_row[j+1]);
                }
            }

            pascal_row = std::move(new_pascal_row);
        }
        return ret;
    }
}
