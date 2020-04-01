#ifndef LINEAR_SOLVER_H
#define LINEAR_SOLVER_H
#include "mtao/types.hpp"
#include <Eigen/Sparse>
#include <memory>

namespace mtao::solvers::linear {
template <typename Matrix, typename Vector>
struct LinearSolverDerived
{
    LinearSolverDerived(const Matrix & A_, const Vector & b_, Vector & x_): A(A_), b(b_), x(x_) {}
    typedef typename Vector::Scalar Scalar;
    virtual void step() = 0;

    protected:
    const Matrix & A;
    const Vector & b;
    Vector & x;
};


template <typename DerivedDerived, typename Matrix, typename Vector>
DerivedDerived createLinearDerived(const Matrix & A, const Vector & b, Vector & x)
{
    return DerivedDerived(A,b,x);
}


template <typename SolverType>
struct solver_traits {
    using Scalar = double;
    using Matrix = Eigen::SparseMatrix<Scalar>;
    using Vector = mtao::VectorX<double>;
};

template <typename Derived>
struct IterativeLinearSolver
{

    Derived& derived() { return *static_cast<Derived*>(this); }
    const Derived& derived() const { return *static_cast<const Derived*>(this); }
    using Traits = solver_traits<Derived>;
    typedef typename Traits::Matrix Matrix;
    typedef typename Traits::Vector Vector;
    typedef typename Traits::Scalar Scalar;
    IterativeLinearSolver(int max_its=1000, Scalar eps=0.001):
        epsilon(eps), max_iterations(max_its) {}


    //Scalar error() const { return derived().error(); }
    virtual Scalar error()
    {
        return (b()-A()*x()).template lpNorm<Eigen::Infinity>();
    }
    void step() { return derived().step(); }
    void solve()
    {
        int iterations = 0;
        while(++iterations < max_iterations &&
                error() > epsilon)
        {
            step();
        }
        //if(iterations >= max_iterations)
        //std::cout << iterations << "/" << max_iterations << ": " << capsule->error() << "/" << epsilon << std::endl;
    }
    void compute() {
        derived().compute();
    }
    void compute(const Matrix & A, const Vector & b) {
        _A = &A;
        _b = &b;
        _x.resize(A.rows());
        compute();
    }
    void compute(const Matrix & A, const Vector & b, const Vector& x) {
        _A = &A;
        _b = &b;
        _x = x;
        compute();
    }
    Vector solve(const Matrix & A, const Vector & b)
    {
        compute(A,b);
        return solve();
    }
    void solve(const Matrix & A, const Vector & b, const Vector & x)
    {
        compute(A,b,x);
        solve();
    }

    const Matrix& A() const { return *_A; }
    const Vector& b() const { return *_b; }
    const Vector& x() const { return _x; }
    Vector& x() { return _x; }
    private:
    const Matrix* _A = nullptr;
    const Vector* _b = nullptr;
    Vector _x;
    Scalar epsilon = 1e-8;
    int max_iterations = 1e3;
};
}

#endif
