#include "mtao/solvers/linear/linear.hpp"

namespace mtao::solvers::linear {
// A = D + U + L
// Ax = Dx + (U+L)x
// 0 = Ax-b = Dx + (U+L)x-b
// 0 = Dx + (U+L)x-b
// Dx = b - (U+L)x
// x = D^{-1}(b - (U+L)x)

template<typename MatrixType>
struct JacobiLinearSolver;
template<typename MatrixType>
struct solver_traits<JacobiLinearSolver<MatrixType>> {
    using Scalar = typename MatrixType::Scalar;
    using Matrix = MatrixType;
    using Vector = mtao::VectorX<Scalar>;
};

template<typename MatrixType>
struct JacobiLinearSolver
  : public IterativeLinearSolver<JacobiLinearSolver<MatrixType>> {
    using Base = IterativeLinearSolver<JacobiLinearSolver<MatrixType>>;
    using Base::A;
    using Base::b;
    using Base::Base;
    using Base::solve;
    using Base::x;

    void compute() {}
    void step() {
        x() = b() - U() * x() - L() * x();
        x() = x().cwiseQuotient(D());
    }
    auto U() const {
        return A().template triangularView<Eigen::StrictlyUpper>();
    }
    auto L() const {
        return A().template triangularView<Eigen::StrictlyLower>();
    }
    auto D() const { return A().diagonal(); }
};

template<typename MatrixType, typename VecType>
auto jacobi(const MatrixType &A, const VecType &b) {
    using Scalar = typename MatrixType::Scalar;
    mtao::VectorX<Scalar> x = mtao::VectorX<Scalar>::Random(A.cols());
    auto residual = (b - A * x).template lpNorm<Eigen::Infinity>();
    auto solver =
      JacobiLinearSolver<MatrixType>(1e6 * A.rows(), 1e-5 * residual);

    solver.solve(A, b, x);
    return solver.x();
}

}// namespace mtao::solvers::linear
