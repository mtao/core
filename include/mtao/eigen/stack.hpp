#pragma once

#include <Eigen/Dense>
#include <tuple>
#include <vector>
#include <type_traits>
#include <utility>
#include <algorithm>


namespace mtao {
namespace eigen {
    template<bool Rows, typename... Args, int... N>
    auto _stack(std::integer_sequence<int, N...>, const Args &... args) {
        using namespace Eigen;
        using Scalar = typename std::tuple_element<0, std::tuple<Args...>>::type::Scalar;

        constexpr static int minCompileRows = std::min<int>({ Args::RowsAtCompileTime... });
        constexpr static int maxCompileRows = std::max<int>({ Args::RowsAtCompileTime... });
        constexpr static int minCompileCols = std::min<int>({ Args::ColsAtCompileTime... });
        constexpr static int maxCompileCols = std::max<int>({ Args::ColsAtCompileTime... });

        constexpr static int sumCompileRows = (Args::RowsAtCompileTime + ... + 0);
        constexpr static int sumCompileCols = (Args::ColsAtCompileTime + ... + 0);

        //constexpr static int myCompRows = (minCompileRows==Dynamic)?Dynamic:(Rows?S:1)*maxCompileRows;
        //constexpr static int myCompCols = (minCompileCols==Dynamic)?Dynamic:(Rows?1:S)*maxCompileCols;
        constexpr static int myCompRows = (minCompileRows == Dynamic) ? Dynamic : (Rows ? sumCompileRows : maxCompileRows);
        constexpr static int myCompCols = (minCompileCols == Dynamic) ? Dynamic : (Rows ? maxCompileCols : sumCompileCols);
        int rows;
        int cols;
        std::vector<int> offset(1, 0);
        auto push_sum = [&](int size) {
            offset.push_back(offset.back() + size);
        };
        if constexpr (Rows) {
            rows = (args.rows() + ... + 0);
            cols = std::max({ args.cols()... });
            (push_sum(args.rows()), ...);
        } else {
            rows = std::max({ args.rows()... });
            cols = (args.cols() + ... + 0);
            (push_sum(args.cols()), ...);
        }


        using Matf = Matrix<Scalar, myCompRows, myCompCols>;
        Matf A = Matf::Constant(rows, cols, 0);


        if constexpr (Rows) {
            (A.block(offset[N], 0, args.rows(), args.cols()).operator=(args), ...);
        } else {
            (A.block(0, offset[N], args.rows(), args.cols()).operator=(args), ...);
        }

        return A;
    }

    template<typename... Args>
    auto vstack(const Args &... args) {
        return _stack<true>(std::make_integer_sequence<int, sizeof...(Args)>(), std::forward<const Args &>(args)...);
    }
    template<typename... Args>
    auto hstack(const Args &... args) {
        return _stack<false>(std::make_integer_sequence<int, sizeof...(Args)>(), std::forward<const Args &>(args)...);
    }


    template<typename BeginIt, typename EndIt>
    auto hstack_iter(BeginIt beginit, EndIt endit) {
        using CDerived = typename std::decay_t<decltype(*beginit)>;

        constexpr static int CRows = CDerived::RowsAtCompileTime;
        using Index = typename CDerived::Scalar;
        using RetCells = Eigen::Matrix<Index, CRows, Eigen::Dynamic>;
        int ccols = 0;
        int crows = 0;

        for (auto it = beginit; it != endit; ++it) {
            auto &&c = *it;
            if (c.size() > 0) {
                crows = std::max<int>(crows, c.rows());
                ccols += c.cols();
            }
        }
        if(crows == 0 || ccols == 0) {
            return RetCells{};
        }
        RetCells mC(crows, ccols);
        ccols = 0;
        for (auto it = beginit; it != endit; ++it) {
            auto &&c = *it;
            if (c.size() > 0) {
                mC.block(0, ccols, c.rows(), c.cols()) = c;
                ccols += c.cols();
            }
        }
        return mC;
    }
    template<typename BeginIt, typename EndIt>
    auto vstack_iter(BeginIt beginit, EndIt endit) {
        using CDerived = typename std::decay_t<decltype(*beginit)>;

        constexpr static int CCols = CDerived::ColsAtCompileTime;
        using Index = typename CDerived::Scalar;
        using RetCells = Eigen::Matrix<Index, Eigen::Dynamic, CCols>;
        int ccols = 0;
        int crows = 0;

        for (auto it = beginit; it != endit; ++it) {
            auto &&c = *it;
            if (c.size() > 0) {
                ccols = std::max<int>(ccols, c.cols());
                crows += c.rows();
            }
        }
        if(crows == 0 || ccols == 0) {
            return RetCells{};
        }
        RetCells mC(crows, ccols);
        crows = 0;
        for (auto it = beginit; it != endit; ++it) {
            auto &&c = *it;
            if (c.size() > 0) {
                mC.block(crows, 0, c.rows(), c.cols()) = c;
                crows += c.rows();
            }
        }
        return mC;
    }
}// namespace eigen
}// namespace mtao
