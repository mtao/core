
#include "mtao/solvers/linear/linear.hpp"
#include <iostream>

namespace mtao::solvers::linear {

template<typename MatrixType, typename VectorType>
class ConjugateGradientLinearSolver;
template<typename MatrixType, typename VectorType>
struct solver_traits<ConjugateGradientLinearSolver<MatrixType, VectorType>> {
    using Scalar = typename MatrixType::Scalar;
    using Matrix = MatrixType;
    using Vector = VectorType;
};

template<typename MatrixType, typename VectorType>
struct ConjugateGradientLinearSolver
  : public IterativeLinearSolver<ConjugateGradientLinearSolver<MatrixType, VectorType>> {
    using Base = IterativeLinearSolver<ConjugateGradientLinearSolver<MatrixType, VectorType>>;
    using Base::A;
    using Base::b;
    using Base::Base;
    using Base::solve;
    using Base::x;
    typedef MatrixType Matrix;
    typedef VectorType Vector;
    typedef typename Vector::Scalar Scalar;
    Scalar error() {
        return rsnorm;
    }

    void compute() {

        r = b() - A() * x();
        std::cout << r.transpose() << std::endl;
        std::cout << x().transpose() << std::endl;
        std::cout << b().transpose() << std::endl;
        p = r;
        Ap = A() * p;
        rsnorm = r.squaredNorm();
        std::cout << rsnorm << std::endl;
    }
    void step() {
        alpha = (rsnorm) / (p.dot(Ap));
        x() += alpha * p;
        r -= alpha * Ap;
        beta = 1 / rsnorm;
        rsnorm = r.squaredNorm();
        beta *= rsnorm;
        p = r + beta * p;
        Ap = A() * p;
    }

  private:
    VectorType r;
    VectorType p;
    VectorType Ap;
    Scalar rsnorm;
    Scalar alpha, beta;
};

template<typename Matrix, typename Vector>
void CGSolve(const Matrix &A, const Vector &b, Vector &x, typename Matrix::Scalar threshold = 1e-5, std::shared_ptr<spdlog::logger> logger = {}, const std::string &name = "cgsolve") {
    auto residual = (b - A * x).template lpNorm<Eigen::Infinity>();
    auto solver = ConjugateGradientLinearSolver<Matrix, Vector>(10 * A.rows(), threshold * residual);
    solver.set_logger(logger);
    solver.set_name(name);
    // auto solver = IterativeLinearSolver<PreconditionedConjugateGradientCapsule<Matrix,Vector, Preconditioner> >(A.rows(), 1e-5);
    solver.solve(A, b, x);
    x = solver.x();
}


}// namespace mtao::solvers::linear
