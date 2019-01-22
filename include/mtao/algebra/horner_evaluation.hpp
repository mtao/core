#pragma once
#include "mtao/iterator/reverse.hpp"
#include "mtao/iterator/zip.hpp"
#include "mtao/type_utils.h"


namespace mtao {
    namespace algebra {

        template <typename T, typename Coefficients>
            auto horner_evaluate(const T& value, const Coefficients& coeffs) {
                using Scalar = mtao::types::remove_cvref_t<T>;
                static_assert(std::is_convertible_v<std::remove_reference_t<typename Coefficients::value_type>,Scalar>);
                Scalar ret{};
                for(auto&& c: coeffs) {
                    ret = ret * value + c;
                }
                return ret;
            }

        template <typename Values, typename Coefficients>
            auto horner_rowminor_index(const Values& values, const Coefficients& coeffs) {
                using T = typename Values::value_type;
                static_assert(std::is_convertible_v<typename Coefficients::value_type,std::remove_cv_t<T>>);
                T ret {};
                using namespace mtao::iterator;
                if constexpr(std::tuple_size_v<Coefficients> == 2) {
                    auto& A = coeffs[0];
                    auto& B = coeffs[1];
                    auto& a = values[0];
                    auto& b = values[1];
                    return a +  A *  b;
                } else if constexpr(std::tuple_size_v<Coefficients> == 3) {
                    auto& A = coeffs[0];
                    auto& B = coeffs[1];
                    auto& C = coeffs[2];
                    auto& a = values[0];
                    auto& b = values[1];
                    auto& c = values[2];
                    return a + A * ( b + B  * c);
                }
                for(auto&& [v,c]: zip(reverse(values),reverse(coeffs))) {
                    ret = ret * c + v;
                }
                return ret;
            }
        template <typename Values, typename Coefficients>
            auto horner_rowmajor_index(const Values& values, const Coefficients& coeffs) {
                using T = typename Values::value_type;
                static_assert(std::is_convertible_v<typename Coefficients::value_type,std::remove_cv_t<T>>);
                T ret{};
                if constexpr(std::tuple_size_v<Coefficients> == 2) {
                    auto& A = coeffs[0];
                    auto& B = coeffs[1];
                    auto& a = values[0];
                    auto& b = values[1];
                    return a * B +  b;
                } else if constexpr(std::tuple_size_v<Coefficients> == 3) {
                    auto& A = coeffs[0];
                    auto& B = coeffs[1];
                    auto& C = coeffs[2];
                    auto& a = values[0];
                    auto& b = values[1];
                    auto& c = values[2];
                    return (a * B +  b) * C  + c;
                }
                using namespace mtao::iterator;
                for(auto&& [v,c]:zip(values,coeffs)) {
                    ret = ret * c + v;
                }
                return ret;
            }
        template <typename T, typename Coefficients>
            auto horner_rowminor_inverse_index(T index, const Coefficients& coeffs) {
                std::array<T,std::tuple_size_v<Coefficients>> ret;
                using namespace mtao::iterator;
                for(auto&& [v,c]:zip(ret,coeffs)) {
                    auto q = std::div(index,c);
                    std::tie(index,v) = std::tie(q.quot,q.rem);
                }
                return ret;
            }
        template <typename T, typename Coefficients>
            auto horner_rowmajor_inverse_index(T index, const Coefficients& coeffs) {
                std::array<T,std::tuple_size_v<Coefficients>> ret;
                using namespace mtao::iterator;
                for(auto&& [v,c]:zip(reverse(ret),reverse(coeffs))) {
                    auto q = std::div(index,c);
                    std::tie(index,v) = std::tie(q.quot,q.rem);
                }
                return ret;
            }

    }
}
