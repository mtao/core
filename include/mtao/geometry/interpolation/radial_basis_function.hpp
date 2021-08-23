#pragma once
#include <cmath>

#include <numbers>
#include "mtao/eigen/shape_checks.hpp"
#include "mtao/types.hpp"

namespace mtao::geometry::interpolation {

namespace detail {
    // each RBF  type should define some basic traits
    template<typename Type>
    struct RBFTraits {};

    template<template<typename> typename Template, typename Scalar_>
    struct RBFTraits<Template<Scalar_>> {
        using Scalar = Scalar_;
    };

    // The common flag "make_pou" detrmines whether a RBF should be assumed to be a
    // partition of unity
    template<typename ScalarFunction, int D>
    class RadialBasisFunction {
      public:
        using Traits = RBFTraits<ScalarFunction>;
        using Scalar = typename Traits::Scalar;
        constexpr static int Dim = D;
        using Vec = mtao::Vector<Scalar, Dim>;
        using RowVecX = mtao::RowVectorX<Scalar>;
        using ColVecs = mtao::ColVectors<Scalar, Dim>;
        template<typename VecDerived, typename CDerived>
        Scalar evaluate(const Eigen::MatrixBase<CDerived> &C, Scalar radius, const Eigen::MatrixBase<VecDerived> &p) const;
        template<typename VecDerived, typename CDerived>
        RowVecX evaluate_multiple(const Eigen::MatrixBase<CDerived> &C,
                                  Scalar radius,
                                  const Eigen::MatrixBase<VecDerived> &p) const;

        template<typename VecDerived, typename CDerived>
        Vec evaluate_grad(const Eigen::MatrixBase<CDerived> &C, Scalar radius, const Eigen::MatrixBase<VecDerived> &p) const;
        template<typename VecDerived, typename CDerived>
        ColVecs evaluate_grad_multiple(
          const Eigen::MatrixBase<CDerived> &C,
          Scalar radius,
          const Eigen::MatrixBase<VecDerived> &p) const;

        // the function on R that generates the RBF
        Scalar f(Scalar v) const { return _function.f(v); }
        // the derivative of the function v
        Scalar df(Scalar v) const { return _function.df(v); }

        Scalar normalization(Scalar radius) const {
            return _function.template normalization<D>(radius);
        }

      private:
        ScalarFunction _function;
    };
    template<typename ScalarFunction, int D>
    template<typename VecDerived, typename CDerived>
    auto RadialBasisFunction<ScalarFunction, D>::evaluate(
      const Eigen::MatrixBase<CDerived> &C,
      Scalar radius,
      const Eigen::MatrixBase<VecDerived> &P) const -> Scalar {
        Scalar distance = (P - C).norm() / radius;
        return f(distance) * normalization(radius);
    }
    template<typename ScalarFunction, int Dim>
    template<typename VecDerived, typename CDerived>
    auto RadialBasisFunction<ScalarFunction, Dim>::evaluate_multiple(
      const Eigen::MatrixBase<CDerived> &C,
      Scalar radius,
      const Eigen::MatrixBase<VecDerived> &P) const -> RowVecX {
        RowVecX R(P.cols());
        mtao::RowVectorX<Scalar> D = (P.colwise() - C).colwise().norm() / radius;
        D.noalias() = D.unaryExpr(
          std::bind(&RadialBasisFunction::f, this, std::placeholders::_1));
        return D * normalization(radius);
    }
    template<typename ScalarFunction, int D>
    template<typename VecDerived, typename CDerived>
    auto RadialBasisFunction<ScalarFunction, D>::evaluate_grad(
      const Eigen::MatrixBase<CDerived> &C,
      Scalar radius,
      const Eigen::MatrixBase<VecDerived> &P) const -> Vec {
        Vec r = P - C;

        Scalar distance = r.norm();
        r /= distance;
        return r * df(distance / radius) * normalization(radius);
    }
    template<typename ScalarFunction, int Dim>
    template<typename VecDerived, typename CDerived>
    auto RadialBasisFunction<ScalarFunction, Dim>::evaluate_grad_multiple(
      const Eigen::MatrixBase<CDerived> &C,
      Scalar radius,
      const Eigen::MatrixBase<VecDerived> &P) const -> ColVecs {
        ColVecs R = (P.colwise() - C);
        RowVecX D = R.colwise().norm();
        R.array().rowwise() /= D.array();
        D /= radius;

        R.noalias() = R * D.unaryExpr(std::bind(&RadialBasisFunction::df, this, std::placeholders::_1)).asDiagonal();
        return R * normalization(radius);
    }

}// namespace detail

template<typename Scalar>
struct HatScalarFunction {
    Scalar f(Scalar x) const { return std::max<Scalar>(0, 1 - std::abs(x)); }
    Scalar df(Scalar x) const { return std::abs(x) > 1 ? 0 : (x < 0 ? 1 : -1); }
    template<int D>
    Scalar normalization(Scalar radius) const {
        return 1;
    }
};
template<typename Scalar, int D>
using HatRadialBasisFunction =
  detail::RadialBasisFunction<HatScalarFunction<Scalar>, D>;

template<typename Scalar>
struct GaussianScalarFunction {
    Scalar f(Scalar v) const { return std::exp(-.5 * v * v); }
    Scalar df(Scalar v) const { return -v * std::exp(-.5 * v * v); }
    template<int D>
    Scalar normalization(Scalar radius) const {
        return std::sqrt(2 * std::numbers::pi_v<Scalar>) * radius;
    }
};

template<typename Scalar, int D>
using GaussianRadialBasisFunction =
  detail::RadialBasisFunction<GaussianScalarFunction<Scalar>, D>;

template<typename Scalar>
struct SplineGaussianScalarFunction {
    Scalar f(Scalar v) const {
        if (v < 1) {
            return 1 - 1.5 * v * v + .75 * v * v * v;
        } else if (v < 2) {
            Scalar t = 2 - v;
            return .25 * (t * t * t);
        }
        return 0;
    }
    Scalar df(Scalar v) const {
        if (v < 1) {
            return 1;
            return -3 * v + 2.25 * v * v;
        } else if (v < 2) {
            return 2;
            Scalar t = 2 - v;
            return .75 * (t * t) * (-v);
        }
        return 0;
    }
    template<int D>
    Scalar normalization(Scalar radius) const {
        return std::numbers::pi_v<Scalar> * radius;
    }
};

template<typename Scalar, int D>
using SplineGaussianRadialBasisFunction =
  detail::RadialBasisFunction<SplineGaussianScalarFunction<Scalar>, D>;

template<typename Scalar>
struct DesbrunSplineScalarFunction {
    Scalar f(Scalar v) const {
        if (v < 2) {
            Scalar t = 2 - v;
            return t * t * t;
        }
        return 0;
    }
    Scalar df(Scalar v) const {
        if (v < 2) {
            Scalar t = 2 - v;
            return 3 * t * t * (-v);
        }
        return 0;
    }
    template<int D>
    Scalar normalization(Scalar radius) const {
        return (std::numbers::pi_v<Scalar> * std::pow(4 * radius, D)) / 15;
    }
};

template<typename Scalar, int D>
using DesbrunSplineRadialBasisFunction =
  detail::RadialBasisFunction<DesbrunSplineScalarFunction<Scalar>, D>;

template<typename PointsType, typename VecType>
auto gaussian_rbf(const Eigen::MatrixBase<PointsType> &P,
                  const Eigen::MatrixBase<VecType> &v,
                  typename PointsType::Scalar radius,
                  bool make_pou = false) {
    using Scalar = typename PointsType::Scalar;
    auto R =
      GaussianRadialBasisFunction<Scalar, PointsType::RowsAtCompileTime>{}
        .evaluate_multiple(v, radius, P);
    if (make_pou) {
        R /= R.sum();
    }
    return R;
}

template<typename PointsType, typename VecType>
auto spline_gaussian_rbf(const Eigen::MatrixBase<PointsType> &P,
                         const Eigen::MatrixBase<VecType> &v,
                         typename PointsType::Scalar radius,
                         bool make_pou = false) {
    using Scalar = typename PointsType::Scalar;
    radius /= 2;
    auto R =
      GaussianRadialBasisFunction<Scalar, PointsType::RowsAtCompileTime>{}
        .evaluate_multiple(v, radius, P);
    if (make_pou) {
        R /= R.sum();
    }
    return R;
}
template<typename PointsType, typename VecType>
auto desbrun_spline_rbf(const Eigen::MatrixBase<PointsType> &P,
                        const Eigen::MatrixBase<VecType> &v,
                        typename PointsType::Scalar radius,
                        bool make_pou = false) {
    using Scalar = typename PointsType::Scalar;
    radius /= 2;
    auto R = DesbrunSplineRadialBasisFunction<Scalar,
                                              PointsType::RowsAtCompileTime>{}
               .evaluate_multiple(v, radius, P);
    if (make_pou) {
        R /= R.sum();
    }
    return R;
}

template<typename PointsType, typename VecType>
auto gaussian_rbg(const Eigen::MatrixBase<PointsType> &P,
                  const Eigen::MatrixBase<VecType> &v,
                  typename PointsType::Scalar radius) {
    using Scalar = typename PointsType::Scalar;
    auto R = radial_basis_function(
      [](Scalar v) -> Scalar { return -v * std::exp(-.5 * v * v); }, P, v, radius);
    R /= (std::sqrt(2 * std::numbers::pi_v<Scalar>) * radius);
    return R;
}

template<typename PointsType, typename VecType>
auto spline_gaussian_rbg(const Eigen::MatrixBase<PointsType> &P,
                         const Eigen::MatrixBase<VecType> &v,
                         typename PointsType::Scalar radius) {
    using Scalar = typename PointsType::Scalar;
    auto R = radial_basis_function(
      [](Scalar v) -> Scalar {
          if (v < 1) {
              return 1 - 3 * v + 2.25 * v * v;
          } else if (v < 2) {
              Scalar t = 2 - v;
              return .75 * (t * t) * (-v);
          }
          return 0;
      },
      P,
      v,
      radius);
    R /= (std::numbers::pi_v<Scalar> * radius);
    return R;
}
template<typename PointsType, typename VecType>
auto desbrun_spline_rbg(const Eigen::MatrixBase<PointsType> &P,
                        const Eigen::MatrixBase<VecType> &v,
                        typename PointsType::Scalar radius) {
    using Scalar = typename PointsType::Scalar;
    auto R = radial_basis_function(
      [](Scalar v) -> Scalar {
          if (v < 2) {
              Scalar t = 2 - v;
              return 3 * t * t * (-v);
          }
          return 0;
      },
      P,
      v,
      radius);
    R *= 15;
    R /= (std::numbers::pi_v<Scalar> * radius * radius * radius * 64);
    return R;
}
}// namespace mtao::geometry::interpolation
