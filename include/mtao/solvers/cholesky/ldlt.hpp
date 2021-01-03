#ifndef LDLT_H
#define LDLT_H
#include <Eigen/Dense>

#include "mtao/type_utils.h"

namespace mtao::solvers::cholesky {

template<typename Matrix>
struct DenseLDLT_MIC0 {
    typedef typename Matrix::Scalar Scalar;
    template<typename A, typename B, typename C>
    inline Scalar tripleVectorProduct(const A &a, const B &b, const C &c) {
        return a.cwiseProduct(b).dot(c);
    }
    inline Scalar tripleProduct(const Matrix &a, uint i, uint j) {
        return tripleVectorProduct(a.row(i).head(j), a.row(j).head(j), a.diagonal().head(j));
    }
    DenseLDLT_MIC0(const Matrix &A) {
        LD = A.template triangularView<Eigen::Lower>();
        int i, j;
        for (i = 0; i < A.rows(); ++i) {
            for (j = 0; j < i; ++j)
                if (std::abs(LD(j, j)) > 0.0001) {
                    LD(i, j) -= tripleProduct(LD, i, j);
                    LD(i, j) /= LD(j, j);
                } else {
                    LD(i, j) = 0;
                }
            LD(i, i) -= tripleProduct(LD, i, i);
        }
    }
    template<typename Vector>
    void solve(const Vector &b, Vector &x) {
        x = LD.template triangularView<Eigen::UnitLower>().solve(b);
        x.noalias() = (LD.diagonal().array().abs() > 1e-5).select(x.cwiseQuotient(LD.diagonal()), x);//safe beacuse it's a dot
        LD.template triangularView<Eigen::UnitLower>().transpose().solveInPlace(x);
    }
    Matrix getA() {
        Matrix A = LD.template triangularView<Eigen::UnitLower>().transpose();
        A = LD.diagonal().asDiagonal() * A;
        A = LD.template triangularView<Eigen::UnitLower>() * A;

        return A;
    }

  private:
    Matrix LD;
};

template<typename Matrix, typename Vector>
struct SparseLDLT_MIC0 {
    typedef typename Matrix::Scalar Scalar;
    SparseLDLT_MIC0() {}
    SparseLDLT_MIC0(const Matrix &A, const Vector &v) : SparseLDLT_MIC0(A) {}
    SparseLDLT_MIC0(const Matrix &A) {
        // L=tril(A);
        L = A.template triangularView<Eigen::StrictlyLower>();// Don't copy
          // the diagonal
        for (int i = 0; i < L.rows(); ++i) {
            if (L.coeff(i, i) != 0) L.coeffRef(i, i) = 0;
        }
        Dinv = D = A.diagonal();

        // for k=1:size(L,2)
        for (int k = 0; k < A.rows();
             ++k)// k is the column that we're infecting the remaining columns
        // with
        {// L(:,k)

            // Solidify the current column values
            //==================================
            if (std::abs(Dinv(k)) < 1e-8) continue;
            //std::cout << "Dinv and D: " << Dinv(k) << " " << D(k) << std::endl;
            if (Dinv(k) <
                0.25 * D(k)) {  // If D has shrunk too much since it started
                //std::cout << "From D" << std::endl;
                Dinv(k) = 1 / D(k);
            } else {
                //std::cout << "From Dinv" << std::endl;
                Dinv(k) = 1 / Dinv(k);
            }
            if(!std::isfinite(Dinv(k))) {
                Dinv(k) = 0;
            }
            //std::cout << k << " " << Dinv(k) << std::endl;
            //            L.innerVector(k) *= Dinv(k);
            L.innerVector(k) = L.innerVector(k) * Dinv(k);

            // Add k terms to all of the following columns
            //===========================================
            for (typename Matrix::InnerIterator it(L, k); it;
                 ++it)// -L(i,k)*D(k)*L(j,k)
            {
                int j = it.row();// j>k
                if (j <= k) continue;
                //spdlog::info("j{} k{}", j,k);
                Scalar missing = 0;
                Scalar multiplier = it.value();// L(j,k)*D(k)
                //spdlog::info("Multiplier!: {}", multiplier);

                typename Matrix::InnerIterator k_it(L, k);
                typename Matrix::InnerIterator j_it(L, j);
                // move down teh column of L(:,k) to collect missing elements in
                // the match with A(:,j) i=k_it.row()

                while (k_it && k_it.row() < j) {// L(i,k)
                    while (j_it)// L(i,j) occasionally
                    {
                        if (j_it.row() < k_it.row())
                            ++j_it;
                        else if (j_it.row() == k_it.row())// L(i,k) are L(i,j) are nonzero
                            break;
                        else {
                            missing += k_it.value();// L(i,k) will fill
                              // something not in L(i,j)
                            break;
                        }
                    }
                    ++k_it;
                }

                if (k_it && j_it.row() == j) {
                    Dinv(j) -= it.value() * multiplier;
                //spdlog::info("Dinv({}) = {} after -= {} {}", j, Dinv(j), it.value(), multiplier);
                }

                typename Matrix::InnerIterator j_it2(L, j);
                while (k_it && j_it2) {
                    if (j_it2.row() < k_it.row())
                        ++j_it2;
                    else if (j_it2.row() == k_it.row())// L(i,k) and L(i,j) are both nonzero,
                    // -=L(i,k)*L(j,k)*D(k)
                    {
                        j_it2.valueRef() -=
                          multiplier * k_it.value();// k_it.value()=L(i,k)
                        ++j_it2;
                        ++k_it;
                    } else {
                        missing += k_it.value();
                        //spdlog::info("Missing {} after += {}", missing, k_it.value());
                        ++k_it;
                    }
                }

                while (k_it) {
                    missing += k_it.value();
                    ++k_it;
                }
                Dinv(j) -= 0.97 * missing * multiplier;
            //spdlog::info("Dinv({}) = {} after -= .97 * {} {}", j, Dinv(j), missing, multiplier);
                //std::cout << L << std::endl;
            }
        }
           //std::cout << "L:\n" << L << std::endl;
        //std::cout << "Final dinv:\n" << Dinv.transpose() << std::endl;
    }
    void solve(const Vector &b, Vector &x) {
        x = L.template triangularView<Eigen::UnitLower>().solve(b);
        x.noalias() = x.cwiseProduct(Dinv);// safe beacuse it's a dot
        L.transpose().template triangularView<Eigen::UnitUpper>().solveInPlace(
          x);
    }
    Matrix getA() {
        Matrix A = L.template triangularView<Eigen::UnitLower>();
        A = A * D.asDiagonal();
        A = A * L.template triangularView<Eigen::UnitLower>().transpose();

        return A;
    }

  private:
    Matrix L;
    Vector D, Dinv;
};

template<typename MatrixType, typename VectorType>
using LDLT_MIC0 = std::conditional_t<
  std::is_base_of_v<Eigen::SparseMatrixBase<MatrixType>, MatrixType>,
  SparseLDLT_MIC0<MatrixType, VectorType>,
  DenseLDLT_MIC0<MatrixType>>;

}// namespace mtao::solvers::cholesky

#endif
