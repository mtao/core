#ifndef PRECONDITIONED_CONJUGATE_GRADIENT_H
#define PRECONDITIONED_CONJUGATE_GRADIENT_H
#include <Eigen/Dense>
#include "mtao/solvers/linear/linear.hpp"
#include "mtao/solvers/cholesky/ldlt.hpp"
#include <iostream>
#include <optional>


namespace mtao::solvers::linear {
template<typename MatrixType, typename VectorType, typename Preconditioner>
struct PCGSolver;

template<typename MatrixType, typename VectorType, typename Preconditioner>
struct solver_traits<PCGSolver<MatrixType, VectorType, Preconditioner>> {
    using Scalar = typename VectorType::Scalar;
    using Matrix = MatrixType;
    using Vector = VectorType;
};


template<typename MatrixType, typename VectorType, typename Preconditioner>
struct PCGSolver : public IterativeLinearSolver<PCGSolver<MatrixType, VectorType, Preconditioner>> {
    typedef MatrixType Matrix;
    typedef VectorType Vector;
    typedef typename Vector::Scalar Scalar;
    using Base = IterativeLinearSolver<PCGSolver<MatrixType, VectorType, Preconditioner>>;
    using Base::A;
    using Base::b;
    using Base::x;
    using Base::Base;
    void compute() {
        precond = Preconditioner(A());
        r = b() - A() * x();
        precond->solve(r, z);
        p = z;
        Ap = A() * p;
        rdz = r.dot(z);
    }
    Scalar error() {
        return r.template lpNorm<Eigen::Infinity>();
    }

    void step() {
        alpha = (rdz) / (p.dot(Ap));
        x() += alpha * p;
        r -= alpha * Ap;
        precond->solve(r, z);
        beta = 1 / rdz;
        rdz = r.dot(z);
        beta *= rdz;
        p = z + beta * p;
        Ap = A() * p;
    }

  private:
    Vector r;
    Vector z;
    Vector p;
    Vector Ap;
    Scalar rdz;
    Scalar alpha, beta;
    std::optional<Preconditioner> precond;
};

template<typename Preconditioner, typename Matrix, typename Vector>
void PCGSolve(const Matrix &A, const Vector &b, Vector &x, typename Matrix::Scalar threshold = 1e-5, std::shared_ptr<spdlog::logger> logger = {}, const std::string &name = "pcgsolve") {
    auto residual = (b - A * x).template lpNorm<Eigen::Infinity>();
    auto solver = PCGSolver<Matrix, Vector, Preconditioner>(5 * A.rows(), threshold * residual);
    solver.set_logger(logger);
    solver.set_name(name);
    // auto solver = IterativeLinearSolver<PreconditionedConjugateGradientCapsule<Matrix,Vector, Preconditioner> >(A.rows(), 1e-5);
    solver.solve(A, b, x);
    x = solver.x();
}


template<typename Matrix, typename Vector>
void DenseCholeskyPCGSolve(const Matrix &A, const Vector &b, Vector &x, typename Matrix::Scalar threshold = 1e-5, std::shared_ptr<spdlog::logger> logger = {}, const std::string &name = "densecholpcgsolve") {
    PCGSolve<cholesky::DenseLDLT_MIC0<std::decay_t<decltype(A)>>>(A, b, x, threshold, logger, name);
}

template<typename Matrix, typename Vector>
void SparseCholeskyPCGSolve(const Matrix &A, const Vector &b, Vector &x, typename Matrix::Scalar threshold = 1e-5, std::shared_ptr<spdlog::logger> logger = {}, const std::string &name = "sparsecholpcgsolve") {
    PCGSolve<cholesky::SparseLDLT_MIC0<Matrix, Vector>>(A, b, x, threshold, logger, name);
}
template<typename Matrix, typename Vector>
void CholeskyPCGSolve(const Matrix &A, const Vector &b, Vector &x, typename Matrix::Scalar threshold = 1e-5, std::shared_ptr<spdlog::logger> logger = {}, const std::string &name = "cholpcgsolve") {
    PCGSolve<cholesky::LDLT_MIC0<Matrix, Vector>>(A, b, x, threshold, logger, name);
}

}// namespace mtao::solvers::linear


#endif
