#ifndef LINEAR_SOLVER_H
#define LINEAR_SOLVER_H
#include <spdlog/spdlog.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "mtao/types.hpp"
#include <spdlog/logger.h>
#include <concepts>
#include "mtao/eigen/shape_checks.hpp"
#include <string>
#include <memory>

namespace mtao::solvers::linear {


namespace concepts {
    template<typename M, typename V>
    concept MultipliableMatrixVectorPair = eigen::concepts::VecCompatible<V> && requires(M m, V v) {
        { m *v } -> std::convertible_to<V>;
    };

}// namespace concepts

template<typename Matrix, typename Vector>
struct LinearSolverDerived {
    LinearSolverDerived(const Matrix &A_, const Vector &b_, Vector &x_) : A(A_), b(b_), x(x_) {}
    typedef typename Vector::Scalar Scalar;
    virtual void step() = 0;

  protected:
    const Matrix &A;
    const Vector &b;
    Vector &x;
};


template<typename DerivedDerived, typename Matrix, typename Vector>
DerivedDerived createLinearDerived(const Matrix &A, const Vector &b, Vector &x) {
    return DerivedDerived(A, b, x);
}


template<typename SolverType>
struct solver_traits {
    using Scalar = double;
    using Matrix = Eigen::SparseMatrix<Scalar>;
    using Vector = mtao::VectorX<double>;
};

// mtao: this is a bad design. Requires more steps to define the interface than to implement the algorithms
template<typename Derived>// requires eigen::concepts::VecCompatible<typename solver_traits<Derived>::Vector> && concepts::MultipliableMatrixVectorPair<typename solver_traits<Derived>::Matrix, typename solver_traits<Derived>::Vector>>
struct IterativeLinearSolver {

    Derived &derived() { return *static_cast<Derived *>(this); }
    const Derived &derived() const { return *static_cast<const Derived *>(this); }
    using Traits = solver_traits<Derived>;
    typedef typename Traits::Matrix Matrix;
    typedef typename Traits::Vector Vector;
    typedef typename Traits::Scalar Scalar;
    IterativeLinearSolver(int max_its = 1000, Scalar eps = 0.001) : epsilon(eps), max_iterations(max_its) {}


    // Scalar error() const { return derived().error(); }
    virtual Scalar error() {
        return (b() - A() * x()).template lpNorm<Eigen::Infinity>();
    }
    void step() { return derived().step(); }
    void solve() {
        int iteration = 0;
        if (bool(logger)) {
            logger->info("{{\"name\":\"{}\", \"max_iteration\":{}, \"error_epsilon\":{}}}", _name, max_iterations, epsilon);
            while (++iteration < max_iterations && error() > epsilon) {
                step();
                logger->info("{{\"name\":\"{}\", \"iteration\":{}, \"error\":{}}}", _name, iteration, error());
            }
            logger->info("{{\"name\":\"{}\", \"iteration\":{}, \"error\":{}, \"converted\": True}}", _name, iteration, error());
        } else {
            while (++iteration < max_iterations && error() > epsilon) {
                step();
            }
        }
        // if(iterations >= max_iterations)
        // std::cout << iterations << "/" << max_iterations << ": " << capsule->error() << "/" << epsilon << std::endl;
    }
    void compute() {
        derived().compute();
    }
    void compute(const Matrix &A, const Vector &b) {
        _A = &A;
        _b = &b;
        _x = Vector::Random(A.rows());
        compute();
    }
    void compute(const Matrix &A, const Vector &b, const Vector &x) {
        _A = &A;
        _b = &b;
        _x = x;
        compute();
    }
    Vector solve(const Matrix &A, const Vector &b) {
        compute(A, b);
        return solve();
    }
    void solve(const Matrix &A, const Vector &b, const Vector &x) {
        compute(A, b, x);
        solve();
    }

    static std::string name() { return Derived::name(); }

    const Matrix &A() const { return *_A; }
    const Vector &b() const { return *_b; }
    const Vector &x() const { return _x; }
    Vector &x() { return _x; }

    void set_name(const std::string &str) {
        _name = str;
    }
    void set_logger(std::shared_ptr<spdlog::logger> l) {
        logger = l;
    }

  private:
    const Matrix *_A = nullptr;
    const Vector *_b = nullptr;
    Vector _x;
    Scalar epsilon = 1e-8;
    int max_iterations = 1e3;
    std::shared_ptr<spdlog::logger> logger = {};
    std::string _name = "unnamed";
};
}// namespace mtao::solvers::linear

#endif
