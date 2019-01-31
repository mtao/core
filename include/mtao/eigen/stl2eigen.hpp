#pragma once
#include "mtao/types.hpp"
#include "mtao/iterator/enumerate.hpp"
namespace mtao { namespace eigen {

    template <template<class> typename Container, typename T, typename = std::enable_if_t<std::is_scalar_v<T>>>
        auto stl2eigen(const Container<T>& vec) {
            mtao::VectorX<T> ret(vec.size());
            for(auto&& [i,v]: mtao::iterator::enumerate(vec)) {
                ret.col(i) = v;
            }

            return ret;
        }
    template <typename T>
        auto stl2eigen(const std::vector<T>& vec) {
            return Eigen::Map<const mtao::VectorX<T>>(vec.data(),vec.size());
        }
    template <typename T, size_t D>
        auto stl2eigen(const std::array<T,D>& vec) {
            return Eigen::Map<const mtao::Vector<T,D>>(vec.data());
        }

    template <template<class,class> typename Container, typename Allocator, typename EigenVec>
        auto stl2eigen(const Container<EigenVec, Allocator>& vec) {
            constexpr int D = EigenVec::RowsAtCompileTime;
            using T = typename EigenVec::Scalar;
            mtao::ColVectors<T,D> ret(D,vec.size());
            for(auto&& [i,arr]: mtao::iterator::enumerate(vec)) {
                ret.col(i) = arr;
            }

            return ret;
        }

    template <template<class,class> typename Container, typename Allocator, typename T, size_t D>
        auto stl2eigen(const Container<std::array<T,D>, Allocator>& vec) {
            mtao::ColVectors<T,D> ret(D,vec.size());
            for(auto&& [i,arr]: mtao::iterator::enumerate(vec)) {
                ret.col(i) = stl2eigen(arr);
            }

            return ret;
        }
    template <template<class,class,class> typename Container, typename Less, typename Allocator, typename T, size_t D>
        auto stl2eigen(const Container<std::array<T,D>, Less, Allocator>& vec) {
            mtao::ColVectors<T,D> ret(D,vec.size());
            for(auto&& [i,arr]: mtao::iterator::enumerate(vec)) {
                ret.col(i) = stl2eigen(arr);
            }

            return ret;
        }
    template <template<class> typename Container, typename T, size_t D>
        auto stl2eigen(const Container<std::array<T,D>>& vec) {
            mtao::ColVectors<T,D> ret(D,vec.size());
            for(auto&& [i,arr]: mtao::iterator::enumerate(vec)) {
                ret.col(i) = stl2eigen(arr);
            }

            return ret;
        }
    template <template<class> typename Container, typename EigenVec>
        auto stl2eigen(const Container<EigenVec>& vec) {
            constexpr int D = EigenVec::RowsAtCompileTime;
            using T = typename EigenVec::Scalar;
            mtao::ColVectors<T,D> ret(D,vec.size());
            for(auto&& [i,arr]: mtao::iterator::enumerate(vec)) {
                ret.col(i) = arr;
            }

            return ret;
        }

}}
