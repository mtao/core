#ifndef EIGEN_UTILS_H
#define EIGEN_UTILS_H
#include <Eigen/Dense>
#include <tuple>
#include <utility>
#include <algorithm>


namespace mtao {

    template <bool Rows, typename... Args, int... N>
        auto _interweave(std::integer_sequence<int,N...>, const Args&... args) {
            using namespace Eigen;
            constexpr static int S = sizeof...(Args);
            using Scalar = typename std::tuple_element<0,std::tuple<Args...>>::type::Scalar;

            constexpr static int minCompileRows = std::min<int>({Args::RowsAtCompileTime...});
            constexpr static int maxCompileRows = std::max<int>({Args::RowsAtCompileTime...});
            constexpr static int minCompileCols = std::min<int>({Args::ColsAtCompileTime...});
            constexpr static int maxCompileCols = std::max<int>({Args::ColsAtCompileTime...});

            constexpr static int myCompRows = (minCompileRows==Dynamic)?Dynamic:(Rows?S:1)*maxCompileRows;
            constexpr static int myCompCols = (minCompileCols==Dynamic)?Dynamic:(Rows?1:S)*maxCompileCols;



            int rows;
            int cols;
            if constexpr(Rows) {
                rows = (args.rows() + ... + 0);
                cols = std::max( {args.cols()...}) ;
            } else {
                rows = std::max( {args.rows()...}) ;
                cols = (args.cols() + ... + 0);
            }

            using Matf = Matrix<Scalar,myCompRows,myCompCols>;
            Matf A = Matf::Constant(rows,cols,0);

            using MatXf = Matrix<Scalar,Dynamic,Dynamic>;
            using MyStride = Stride<Dynamic,Dynamic>;

            if constexpr(Rows) {
                (Map<MatXf, 0,MyStride>(A.data() + N, args.rows(),args.cols(), MyStride(S*args.rows(),S)).operator=(args),...);
            } else {
                (Map<MatXf, 0,MyStride>(A.data() + N * rows, args.rows(),args.cols(), MyStride(S,1)).operator=(args),...);
            }

            return A;
        }
    template <typename... Args>
        auto interweaveRows(const Args&... args) {
            return _interweave<true>(std::make_integer_sequence<int,sizeof...(Args)>(), std::forward<const Args&>(args)...);
        }
    template <typename... Args>
        auto interweaveCols(const Args&... args) {
            return _interweave<false>(std::make_integer_sequence<int,sizeof...(Args)>(), std::forward<const Args&>(args)...);
        }
}

#endif//EIGEN_UTILS_H
