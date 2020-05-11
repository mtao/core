#include "mtao/solvers/linear/linear.hpp"

namespace mtao::solvers::linear {
// A = D + L + R
// Ax = Lx + (D+R)x
// 0 = Ax-b = Lx + (D+R)x-b
// 0 = Lx + (D+R)x-b
// Lx = b - (L+R)x
// x = L^{-1}(b - (D+R)x)

template <typename MatrixType>
class GaussSeidelLinearSolver;
template <typename MatrixType>
struct solver_traits<GaussSeidelLinearSolver<MatrixType>> {
    using Scalar = typename MatrixType::Scalar;
    using Matrix = MatrixType;
    using Vector = mtao::VectorX<Scalar>;
};

template <typename MatrixType>
    struct GaussSeidelLinearSolver
    : public IterativeLinearSolver<GaussSeidelLinearSolver<MatrixType> > {
    using Base = IterativeLinearSolver<GaussSeidelLinearSolver<MatrixType>>;
    using Base::Base;
    using Base::A;
    using Base::b;
    using Base::x;
    using Base::solve;
    void compute() {}
    void step() {
        x() = b() - US() * x();
        L().solveInPlace(x());
    }
    auto US() const { return A().template triangularView<Eigen::StrictlyUpper>(); }
    auto L() const { return A().template triangularView<Eigen::Lower>(); }
};

template <typename MatrixType, typename VecType>
auto gauss_seidel(const MatrixType& A, const VecType& b) {
    using Scalar = typename MatrixType::Scalar;
    mtao::VectorX<Scalar> x(A.cols());
    auto residual = (b-A*x).template lpNorm<Eigen::Infinity>();
    auto solver = GaussSeidelLinearSolver<MatrixType>(1e6*A.rows(), 1e-5*residual);
    solver.solve(A,b,x);
    return solver.x();
}
}  // namespace mtao::solvers::linear
