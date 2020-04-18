#pragma once
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <functional>

namespace mtao::quadrature {
// N = number of intervals integrated, N+1 calls made
template <typename Scalar = double,
          typename Func = std::function<double(double)> >
Scalar simpsons_rule(Func&& f, Scalar a, Scalar b, int N = 3);

template <int D, typename Scalar = double, typename Func = void>
Scalar multidim_simpsons_rule(Func&& f, Scalar min, Scalar max, int count = 3);

template <int D, typename Scalar = double, typename Func = void>
Scalar multidim_simpsons_rule(Func&& f, const std::array<Scalar, D>& min,
                              const std::array<Scalar, D>& max,
                              const std::array<int, D>& count);

template <typename Scalar, typename Func>
Scalar simpsons_rule(Func&& f, Scalar a, Scalar b, int N) {
    if (N == 0) {
        return (b - a) * (f((a + b) / 2.));
    } else if (N == 1) {
        return ((b - a) / Scalar(2)) * (f(a) + f(b));
    } else if (N == 2) {
        return ((b - a) / Scalar(6)) * (f(a) + 4 * f((a + b) / (2.)) + f(b));
    } else if (N == 3) {
        return ((b - a) / Scalar(8)) *
               (f(a) + 3 * (f((2 * a + b) / 3.) + f((a + 2 * b) / 3.)) + f(b));
    } else if (N % 3 == 0) {
        Scalar h = (b - a) / N;
        Scalar val = f(a) + f(b);
        for (int j = 1; j < N; ++j) {
            val += (j % 3 == 0 ? 2 : 3) * f(a + j * h);
        }
        return (3 * h / 8) * val;
    } else if (N % 2 == 0) {
        Scalar h = (b - a) / N;
        Scalar val = f(a) + f(b);
        for (int j = 1; j < N; ++j) {
            val += (j % 2 == 0 ? 2 : 4) * f(a + j * h);
        }
        return h * val / 3;
    } else {
        spdlog::info(
            "simpsons rule on an unhandled case, going up a sample and trying "
            "again");
        return simpsons_rule(f, a, b, N + 1);
    }
}

template <int D, typename Scalar, typename Func>
Scalar multidim_simpsons_rule(Func&& f, Scalar min, Scalar max, int count) {
    std::array<Scalar, D> minA, maxA;
    std::array<int, D> countA;
    std::fill(minA.begin(), minA.end(), min);
    std::fill(maxA.begin(), maxA.end(), max);
    std::fill(countA.begin(), countA.end(), count);
    return multidim_simpsons_rule<D>(f, minA, maxA, countA);
}
namespace internal {
template <int D, int LD = 0, typename Func = void, typename Scalar = double>
Scalar multidim_simpsons_rule(Func&& f, const std::array<Scalar, D>& min,
                              const std::array<Scalar, D>& max,
                              const std::array<int, D>& count,
                              std::array<Scalar, D>& thusfar);
}

template <int D, typename Scalar = double, typename Func = void>
Scalar multidim_simpsons_rule(Func&& f, const std::array<Scalar, D>& min,
                              const std::array<Scalar, D>& max,
                              const std::array<int, D>& count) {
    std::array<Scalar, D> thusfar = min;
    return internal::multidim_simpsons_rule<D>(f, min, max, count, thusfar);
}
namespace internal {
template <int D, int LD, typename Func, typename Scalar>
Scalar multidim_simpsons_rule(Func&& f, const std::array<Scalar, D>& min,
                              const std::array<Scalar, D>& max,
                              const std::array<int, D>& count,
                              std::array<Scalar, D>& thusfar) {
    if constexpr (LD > D - 1) {
        Scalar v = f(thusfar);
        return v;
    } else {
        return quadrature::simpsons_rule(
            [&](Scalar a) -> Scalar {
                thusfar[LD] = a;
                Scalar v = multidim_simpsons_rule<D, LD + 1>(f, min, max, count,
                                                             thusfar);
                return v;
            },
            min[LD], max[LD], count[LD]);
    }
}
}  // namespace internal

}  // namespace mtao::quadrature
