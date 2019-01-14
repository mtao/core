#pragma once
#include <mtao/types.hpp>

namespace mtao { namespace eigen { namespace preconditioners {
    //Modified incomplete Cholesky Level 0
    template <typename _Scalar>
        class MIC0Preconditioner
        {
            using Scalar = _Scalar;
            using Vector = mtao::VectorX<Scalar>;
            using SparseMatrix = Eigen::SparseMatrix<Scalar>;
            using Index = int;
            public:
            typedef typename Vector::StorageIndex StorageIndex;
            enum {
                ColsAtCompileTime = Eigen::Dynamic,
                MaxColsAtCompileTime = Eigen::Dynamic
            };

            MIC0Preconditioner() : m_isInitialized(false) {}

            template<typename MatType>
                explicit MIC0Preconditioner(const MatType& mat) : Dinv(mat.cols())
            {
                compute(mat);
            }

            Index rows() const { return L.rows(); }
            Index cols() const { return L.rows(); }

            template<typename MatType>
                MIC0Preconditioner& analyzePattern(const MatType& )
                {
                    return *this;
                }

            template<typename MatType>
                MIC0Preconditioner& factorize(const MatType& mat)
                {
                    Vector D;
                    // L=tril(A);
                    L=mat.template triangularView<Eigen::StrictlyLower>();//Don't copy the diagonal
                    for(int i=0; i<L.rows(); ++i)
                    {
                        if(L.coeff(i,i)!=0)
                            L.coeffRef(i,i)=0;
                    }
                    if constexpr(false) {
                        Dinv=D=mat.diagonal();
                    } else {
                        Dinv.resize(mat.cols());
                        for(int j=0; j<mat.outerSize(); ++j)
                        {
                            typename MatType::InnerIterator it(mat,j);
                            while(it && it.index()!=j) ++it;
                            if(it && it.index()==j && it.value()!=Scalar(0))
                                Dinv(j) = Scalar(1)/it.value();
                            else
                                Dinv(j) = Scalar(1);
                        }
                        D = Dinv;
                    }



                    // for k=1:size(L,2)
                    for(int k=0; k<mat.rows(); ++k)//k is the column that we're infecting the remaining columns with
                    {//L(:,k)



                        //Solidify the current column values
                        //==================================
                        if(std::abs(D(k)) < 1e-5) continue;
                        if(Dinv(k)<0.25*D(k)) {//If D has shrunk too much since it started
                            Dinv(k)=1/D(k);
                        } else {
                            Dinv(k)=1/Dinv(k);
                        }
                        L.innerVector(k) *= Dinv(k);

                        //Add k terms to all of the following columns
                        //===========================================
                        for(typename SparseMatrix::InnerIterator it(L,k); it; ++it)// -L(i,k)*D(k)*L(j,k)
                        {
                            int j = it.row();//j>k
                            if(j<=k) continue;
                            Scalar missing=0;
                            Scalar multiplier=it.value();//L(j,k)*D(k)

                            typename SparseMatrix::InnerIterator k_it(L,k);
                            typename SparseMatrix::InnerIterator j_it(L,j);
                            //move down teh column of L(:,k) to collect missing elements in the match with A(:,j)
                            //i=k_it.row()

                            while (k_it && k_it.row()<j){//L(i,k)
                                while(j_it)//L(i,j) occasionally
                                {
                                    if(j_it.row() < k_it.row())
                                        ++j_it;
                                    else if(j_it.row() == k_it.row())//L(i,k) are L(i,j) are nonzero
                                        break;
                                    else
                                    {
                                        missing += k_it.value();//L(i,k) will fill something not in L(i,j)
                                        break;
                                    }
                                }
                                ++k_it;
                            }


                            if(k_it && j_it.row() == j)
                            {
                                Dinv(j) -= it.value() * multiplier;
                            }


                            typename SparseMatrix::InnerIterator j_it2(L,j);
                            while(k_it && j_it2)
                            {
                                if(j_it2.row() < k_it.row())
                                    ++j_it2;
                                else if(j_it2.row() == k_it.row())//L(i,k) and L(i,j) are both nonzero, -=L(i,k)*L(j,k)*D(k)
                                {
                                    j_it2.valueRef() -= multiplier * k_it.value() ;//k_it.value()=L(i,k)
                                    ++j_it2;
                                    ++k_it;
                                }
                                else
                                {
                                    missing+=k_it.value();
                                    ++k_it;
                                }
                            }

                            while(k_it)
                            {
                                missing+=k_it.value();
                                ++k_it;
                            }
                            Dinv(j)-=0.97*missing*multiplier;
                        }
                    }

                    m_isInitialized = true;
                    return *this;
                }

            template<typename MatType>
                MIC0Preconditioner& compute(const MatType& mat)
                {
                    return factorize(mat);
                }

            template<typename Rhs, typename Dest>
                void _solve_impl(const Rhs& b, Dest& x) const
                {
                    x = L.template triangularView<Eigen::UnitLower>().solve(b);
                    x = Dinv.cwiseProduct(x);//safe beacuse it's a dot
                    L.transpose().template triangularView<Eigen::UnitUpper>().solveInPlace(x);
                }

            template<typename Rhs> inline const Eigen::Solve<MIC0Preconditioner, Rhs>
                solve(const Eigen::MatrixBase<Rhs>& b) const
                {
                    eigen_assert(m_isInitialized && "MIC0Preconditioner is not initialized.");
                    eigen_assert(L.rows()==b.rows()
                            && "MIC0Preconditioner::solve(): invalid number of rows of the right hand side matrix b");
                    return Eigen::Solve<MIC0Preconditioner, Rhs>(*this, b.derived());
                }

            Eigen::ComputationInfo info() { return Eigen::Success; }

            protected:
            SparseMatrix L;
            Vector Dinv;
            bool m_isInitialized;
        };
}}}
