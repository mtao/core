#pragma once
#include "mtao/types.hpp"
#include "mtao/iterator/enumerate.hpp"
#include <bitset>
namespace mtao { namespace eigen {

    namespace internal {
    template <typename T>
        constexpr int container_size() {
            if constexpr(mtao::types::has_tuple_size<T>()) {
                return std::tuple_size<T>();
            } else {
                return Eigen::Dynamic;
            }
        }
    template <typename T>
        constexpr int container_size(T&&) {
            return container_size<T>();
        }
    }
    template <typename Derived>
        auto stl2eigen(const Eigen::EigenBase<Derived>& vec) {
            return vec;
        }
    template <typename T, typename Allocator, typename = std::enable_if_t<std::is_scalar_v<T>>>
        auto stl2eigen(const std::vector<T, Allocator>& vec) {
            return Eigen::Map<const mtao::VectorX<T>>(vec.data(),vec.size());
        }
    template <typename T, size_t D, typename = std::enable_if_t<std::is_scalar_v<T>>>
        auto stl2eigen(const std::array<T,D>& vec) {
            return Eigen::Map<const mtao::Vector<T,D>>(vec.data());
        }
    template <size_t D>
        auto stl2eigen(const std::bitset<D>& bs) {
            mtao::Vector<bool,D> v;
                for(int j = 0; j < D; ++j) {
                v(j) = bs[j];
                }
                return v;

        }


    template <typename Container>
        auto stl2eigen(const Container& vec) {

            constexpr static int CSize = internal::container_size<Container>();
            using InnerType = typename Container::value_type;
            if constexpr(std::is_base_of_v<Eigen::MatrixBase<InnerType>,InnerType>) {//internal eigen matrix
                using T = typename InnerType::Scalar;
                constexpr int D = InnerType::RowsAtCompileTime;
                static_assert(D != Eigen::Dynamic, "To guarantee constant sizes we only support converting containers of constant size Eigen Vectors");
                mtao::Matrix<T,D,CSize> ret(D,vec.size());
                for(auto&& [i,arr]: mtao::iterator::enumerate(vec)) {
                    ret.col(i) = arr;
                }
                return ret;
            } else if constexpr(std::is_scalar_v<InnerType>) {//scalar so vector
                using T = InnerType;
                mtao::Vector<T,CSize> ret(vec.size());
                using namespace mtao::iterator;
                std::copy(vec.begin(),vec.end(),ret.data());
                return ret;
            } else {//filled with another container
                constexpr int D = internal::container_size<InnerType>();
                using T = typename InnerType::value_type;
                static_assert(D != Eigen::Dynamic, "To guarantee constant sizes we only support converting containers of constant size Eigen Vectors");

                mtao::ColVectors<T,D> ret(D,vec.size());
                for(auto&& [i,arr]: mtao::iterator::enumerate(vec)) {
                    ret.col(i) = stl2eigen(arr);
                }

                return ret;
            }
        }

}}
