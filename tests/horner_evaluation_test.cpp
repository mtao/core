#include <iostream>
#include <array>
#include <mtao/algebra/horner_evaluation.hpp>
#include "mtao/geometry/grid/grid_utils.h"
#include <Eigen/Dense>
#include <iterator>



int main() {
    std::array<int,3> coeffs{{1,1,0}};
    for(int i = 0; i < 10; ++i) {
        std::cout << mtao::algebra::horner_evaluate(i,coeffs) << std::endl;
    }
    {
        std::array<int,2> coeffs{{3,4}};
        for(int i = 0; i < coeffs[0]; ++i) {
            for(int j = 0; j < coeffs[1]; ++j) {
                std::cout << mtao::algebra::horner_rowminor_index(std::array<int,2>{{i,j}},coeffs) << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl << std::endl;
    {
        std::array<int,2> coeffs{{3,4}};
        for(int i = 0; i < coeffs[0]; ++i) {
            for(int j = 0; j < coeffs[1]; ++j) {
                std::cout << mtao::algebra::horner_rowmajor_index(std::array<int,2>{{i,j}},coeffs) << " ";
            }
            std::cout << std::endl;
        }
    }
    {
        std::array<int,3> coeffs{{3,4,5}};
        for(int i = 0; i < coeffs[0]; ++i) {
            for(int j = 0; j < coeffs[1]; ++j) {
                for(int k = 0; k < coeffs[2]; ++k) {
                    std::cout << mtao::algebra::horner_rowminor_index(std::array<int,3>{{i,j,k}},coeffs) << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl << std::endl;
    {
        std::array<int,3> coeffs{{3,4,5}};
        for(int i = 0; i < coeffs[0]; ++i) {
            for(int j = 0; j < coeffs[1]; ++j) {
                for(int k = 0; k < coeffs[2]; ++k) {
                    std::cout << mtao::algebra::horner_rowmajor_index(std::array<int,3>{{i,j,k}},coeffs) << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }

    {
        Eigen::Matrix<int,3,4,Eigen::ColMajor> A;
        for(int i = 0; i < A.rows(); ++i) {
            for(int j = 0; j < A.cols(); ++j) {
                A(i,j) = &A(i,j) - A.data();

            }
        }
        std::array<int,2> coeffs{{3,4}};
        for(int i = 0; i < coeffs[0]; ++i) {
            for(int j = 0; j < coeffs[1]; ++j) {
                std::cout << mtao::algebra::horner_rowminor_index(std::array<int,2>{{i,j}},coeffs) << ":" << A(i,j) << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl << std::endl;
    {
        Eigen::Matrix<int,3,4,Eigen::RowMajor> A;
        for(int i = 0; i < A.rows(); ++i) {
            for(int j = 0; j < A.cols(); ++j) {
                A(i,j) = &A(i,j) - A.data();

            }
        }
        std::array<int,2> coeffs{{3,4}};
        for(int i = 0; i < coeffs[0]; ++i) {
            for(int j = 0; j < coeffs[1]; ++j) {
                std::cout << mtao::algebra::horner_rowmajor_index(std::array<int,2>{{i,j}},coeffs)  << ":" << A(i,j) << " ";
            }
            std::cout << std::endl;
        }
    }


    auto arrstr = [](auto&& idx) {
        using ArrType = mtao::types::remove_cvref_t<decltype(idx)>;
        using VT = typename ArrType::value_type;
        std::stringstream ss;
        
        std::copy(idx.begin(),idx.end(),std::ostream_iterator<VT>(ss,","));
        return ss.str();

    };

    using namespace mtao::algebra;
    auto test_inversing = [&](auto&& coeffs) {
        std::cout << "Minor" << coeffs.size() << std::endl;
        mtao::geometry::grid::utils::multi_loop(coeffs,[&](auto&& ij) {
                int idx = horner_rowminor_index(ij,coeffs);
                auto arr = horner_rowminor_inverse_index(idx,coeffs);
                std::cout << arrstr(ij) << " => " << idx << " => " << arrstr(arr) << std::endl;
                assert(ij == arr);
                });
        std::cout << std::endl;
        std::cout << "Major" << coeffs.size() << std::endl;
        mtao::geometry::grid::utils::multi_loop(coeffs,[&](auto&& ij) {
                int idx = horner_rowmajor_index(ij,coeffs);
                auto arr = horner_rowmajor_inverse_index(idx,coeffs);
                std::cout << arrstr(ij) << " => " << idx << " => " << arrstr(arr) << std::endl;
                assert(ij == arr);
                });
    };
    test_inversing(std::array<int,2>{{3,4}});
    std::cout << std::endl;
    std::cout << std::endl;
    test_inversing(std::array<int,3>{{3,4,5}});
    std::cout << std::endl;
    std::cout << std::endl;
    test_inversing(std::array<int,4>{{3,4,5,7}});




}
