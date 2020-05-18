#include <iostream>

#include "mtao/eigen/type_info.hpp"
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
    : public IterativeLinearSolver<GaussSeidelLinearSolver<MatrixType>> {
    using Base = IterativeLinearSolver<GaussSeidelLinearSolver<MatrixType>>;
    using Base::A;
    using Base::b;
    using Base::Base;
    using Base::solve;
    using Base::x;
    using Scalar = typename Base::Scalar;
    using Vec = mtao::VectorX<Scalar>;
    void compute() {
        invdiag = A().diagonal();
        invdiag = (invdiag.array() > std::numeric_limits<Scalar>::epsilon())
                      .select(1. / invdiag.array(), Scalar(0));
    }
    void step() {
        // x() = b() - US() * x();
        // L().solveInPlace(x());
        // if constexpr (eigen::is_sparse<MatrixType>()) {
        //} else {  // eigen::is_dense<MatrixType>()
#if defined(_MTAO_GAUSS_SEIDEL_CUSTOM_ERROR)
        _error = 0;
#endif
        for (int i = 0; i < A().rows(); ++i) {
            auto Ar = A().row(i);
            Scalar& v = x()(i);
#if defined(_MTAO_GAUSS_SEIDEL_CUSTOM_ERROR)
            Scalar old = v;
#endif
            v = b()(i);
            v -= Ar.head(i).dot(x().head(i));
            v -= Ar.tail(Ar.size() - i - 1).dot(x().tail(Ar.size() - i - 1));
            v *= invdiag(i);
#if defined(_MTAO_GAUSS_SEIDEL_CUSTOM_ERROR)
            Scalar diff = std::abs(v - old);
            _error = std::max(diff, _error);
#endif
        }
        //}
    }

#if defined(_MTAO_GAUSS_SEIDEL_CUSTOM_ERROR)
    virtual error() const override { return _error; }
#endif
    auto US() const {
        return A().template triangularView<Eigen::StrictlyUpper>();
    }
    auto L() const { return A().template triangularView<Eigen::Lower>(); }

    Vec invdiag;
    Scalar _error = std::numeric_limits<Scalar>::max();
};

template <typename MatrixType, typename VecType>
auto gauss_seidel(const MatrixType& A, const VecType& b) {
    using Scalar = typename MatrixType::Scalar;
    mtao::VectorX<Scalar> x(A.cols());
    auto residual = (b - A * x).template lpNorm<Eigen::Infinity>();
    auto solver =
        GaussSeidelLinearSolver<MatrixType>(1e6 * A.rows(), 1e-5 * residual);
    solver.solve(A, b, x);
    return solver.x();
}
}  // namespace mtao::solvers::linear
