#pragma once
#include "mtao/eigen/partition_matrix.hpp"
#include "mtao/eigen/partition_vector.hpp"
namespace mtao::eigen {
/*
    std::vector ind{ 0, 2 };
    auto inv = mtao::eigen::inverse_mask(M.rows(), ind);
    print_nothing(inv);
    auto ICPi = mtao::eigen::mask_matrix<double>(M.rows(), inv);
    auto CPi = mtao::eigen::projected_mask_matrix<double>(M.rows(), inv);


    auto IPi = mtao::eigen::mask_matrix<double>(M.rows(), ind);
    auto Pi = mtao::eigen::projected_mask_matrix<double>(M.rows(), ind);

    // M (Pi + NPi)x = b
    // x_0 0

    auto A = Pi * MM * Pi.transpose();
    auto B = Pi * MM * CPi.transpose();
    auto C = CPi * MM * Pi.transpose();
    auto D = CPi * MM * CPi.transpose();
    auto a = Pi * RHS;
    auto b = CPi * RHS;

    mtao::VecXd tb = a - B * D.ldlt().solve(b);
    Eigen::MatrixXd tM = A - B * D.ldlt().solve(C);
    Eigen::VectorXd x = tM.ldlt().solve(tb);
    Eigen::VectorXd y = D.ldlt().solve(b - C * x);

    Eigen::VectorXd X = Pi.transpose() * x + CPi.transpose() * y;


template<typename T>
struct SchurComplement {

    void compute_projectors(const Eigen::SparseMatrix<T> &M, const std::set<int> &active_set) {
        auto inv = mtao::eigen::inverse_mask(M.rows(), ind);
        Pi = mtao::eigen::projected_mask_matrix<double>(M.rows(), active_set);
        CPi = mtao::eigen::projected_mask_matrix<double>(M.rows(), inv);
    }

    // Ax + By = a
    // Cx + Dy = B

    template <typename SolverFunc>
    std::tuple<Eigen::SparseMatrix<T>, mtao::VectorX<T>> void step1(const Eigen::SparseMatrix<T>& A, SolverFunc&& solve = [

    void invert_active_set() {
        std::swap(Pi, CPi);
    }

    void compute(const Eigen::SparseMatrix<T> &A, const std::set<int> &active_set);


    Eigen::SparseMatrix<T> Pi;
    Eigen::SparseMatrix<T> CPi;
};
*/


}// namespace mtao::eigen
