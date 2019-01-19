#include <iostream>
#include <array>
#include <mtao/algebra/horner_evaluation.hpp>
#include <Eigen/Dense>


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
}
