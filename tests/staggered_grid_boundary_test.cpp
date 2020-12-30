#include <mtao/geometry/grid/staggered_grid.hpp>
#include <iostream>
#include <numeric>
#include <iterator>
#include <array>
#include <mtao/iterator/enumerate.hpp>
#include <mtao/iterator/reverse.hpp>
#include <catch2/catch.hpp>

using namespace mtao;

template<int D, typename SGType>
void hodge(const SGType &sg, mtao::VecXf &h, int N) {
    constexpr static int Dim = SGType::D;
    auto g = sg.template grid<D>(N);
    int M = N;
    if (Dim == 2 && D == 1) {
        N = 1 - N;
        M = 1 - N;
    }
    int mask = mtao::geometry::grid::utils::internal::full_mask(Dim) - (1 << N);
    //if(D == 0) {
    //mask = mtao::geometry::grid::utils::internal::full_mask(Dim) - (1 << 0);
    //}
    mtao::geometry::grid::utils::masked_multi_loop(
      g.shape(), [&](auto c) {
          c[N] = 0;
          h(sg.template staggered_index<D>(c, M)) = 0;
          //std::copy(c.begin(),c.end(),std::ostream_iterator<int>(std::cout,":"));
          //std::cout << sg.template staggered_index<D>(c,M) << std::endl;
          c[N] = g.shape()[N] - 1;
          h(sg.template staggered_index<D>(c, M)) = 0;
          //std::copy(c.begin(),c.end(),std::ostream_iterator<int>(std::cout,":"));
          //std::cout << sg.template staggered_index<D>(c,M) << std::endl;
      },
      mask);
    //if(D == 0) {
    //    N = 1;
    //    int mask = mtao::geometry::grid::utils::internal::full_mask(Dim) - (1 << 1);
    //mtao::geometry::grid::utils::masked_multi_loop(g.shape(),[&](auto c) {

    //        c[N] = 0;
    //        h(sg.template staggered_index<D>(c,0)) = 0;
    //        c[N] = g.shape()[N]-1;
    //        h(sg.template staggered_index<D>(c,0)) = 0;
    //        //std::copy(c.begin(),c.end(),std::ostream_iterator<int>(std::cout,":"));
    //        //std::cout << sg.template staggered_index<D>(c,0) << std::endl;
    //        },mask);
    //}
}
template<int D, typename SGType>
mtao::VecXf hodge(const SGType &sg) {
    constexpr static int Dim = SGType::D;
    mtao::VecXf h(sg.template form_size<D>());
    h.setOnes();

    int ND = mtao::combinatorial::nCr(Dim, D);
    for (int j = 0; j < ND; ++j) {
        hodge<D>(sg, h, j);
    }
    return h;
}

template<int D, typename SGType>
Eigen::SparseMatrix<float> d(const SGType &sg) {
    return hodge<D - 1>(sg).asDiagonal() * sg.template boundary<D>();
}
template<int D, typename SGType>
auto dd(const SGType &sg) {
    Eigen::SparseMatrix<float> r = d<D>(sg) * d<D + 1>(sg);
    return r;
}
template<typename SMat>
auto infnorm(const SMat &S) {
    using Scalar = typename SMat::Scalar;
    using Vec = mtao::VectorX<Scalar>;
    return Vec::Ones(S.rows()).transpose() * S.cwiseAbs() * Vec::Ones(S.cols());
}
template<typename SMat>
auto sums(const SMat &S) {
    using Scalar = typename SMat::Scalar;
    using Vec = mtao::VectorX<Scalar>;
    return Vec::Ones(S.rows()).transpose() * (S * Vec::Ones(S.cols())).cwiseAbs();
}
TEST_CASE("Staggered Grid boundary 2D", "[grid][staggered_grid]") {
    constexpr static int D = 2;
    std::array<int, D> arr;
    std::iota(arr.begin(), arr.end(), 3);
    mtao::geometry::grid::StaggeredGrid<float, D> sg(arr);
    //REQUIRE(infnorm(dd<1>(sg))== 0);//2*(3+4));
    REQUIRE(sums(d<2>(sg)) == 0);
}
TEST_CASE("Staggered Grid boundary 3D", "[grid][staggered_grid]") {
    constexpr static int D = 3;
    std::array<int, D> arr;
    std::iota(arr.begin(), arr.end(), 3);
    mtao::geometry::grid::StaggeredGrid<float, D> sg(arr);
    //REQUIRE(infnorm(dd<2>(sg)) == 0);//2*(3*4 + 3*5 + 4*5));
    REQUIRE(sums(d<3>(sg)) == 0);
}
