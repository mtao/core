#pragma once
#include "mtao/iterator/reverse.hpp"
#include "mtao/iterator/zip.hpp"


namespace mtao {
    namespace algebra {

        template <typename T, typename Coefficients>
            auto horner_evaluate(const T& value, const Coefficients& coeffs) {
                using Scalar = std::remove_reference_t<std::remove_cv_t<T>>;
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
                using namespace mtao::iterator;
                for(auto&& [v,c]:zip(values,coeffs)) {
                    ret = ret * c + v;
                }
                return ret;
            }

    }
}
