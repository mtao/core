#pragma once

#include "mtao/types.h"
#include <iostream>


namespace mtao { namespace eigen {

    template <bool Row=false,typename Derived>
        auto slice_filter(const Eigen::MatrixBase<Derived>& A, const mtao::VectorX<bool>& M) {

            using Scalar = typename Derived::Scalar;
            constexpr static int  RACT = Row ? Derived::RowsAtCompileTime : Eigen::Dynamic;
            constexpr static int  CACT = Row ? Eigen::Dynamic: Derived::ColsAtCompileTime;

            int size = M.count();

            using Mat = Eigen::Matrix<Scalar,RACT,CACT>;
            std::cout << RACT << " = " << CACT << std::endl;
            std::cout  << A.rows() << "/" << A.cols() << std::endl;
            std::cout << "Size: "  << (Row?size:A.rows()) << " " << (Row?A.cols():size) << std::endl;
            Mat B( Row?size:A.rows(),Row?A.cols():size);

            std::cout << "B: " << B.rows() << " ? " << B.cols() << std::endl;

            int c = 0;
            for(int i = 0; i < (Row?A.rows():A.cols()); ++i) {
                std::cout << i << ") " << M(i) << std::endl;
                if(M(i)) {
                        std::cout << A.row(i) << std::endl;
                        std::cout << B.row(c) << std::endl;
                    if constexpr(Row) {
                    B.row(c++) = A.row(i);
                    } else {
                    B.col(c++) = A.col(i);
                    }
                }
            }
            return B;
        }
    template <typename Derived>
        auto row_slice_filter(const Eigen::MatrixBase<Derived>& A, const mtao::VectorX<bool>& M) {
            return slice_filter<true>(A,M);
        }

    template <typename Derived>
        auto col_slice_filter(const Eigen::MatrixBase<Derived>& A, const mtao::VectorX<bool>& M) {
            return slice_filter<false>(A,M);
        }

}}
