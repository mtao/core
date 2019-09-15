#include "curve.hpp"
#include <mtao/eigen/stack.h>
#include <mtao/eigen/stl2eigen.hpp>
#include <mtao/geometry/interpolation/mean_value.hpp>
#include <mtao/geometry/interpolation/wachpress.hpp>
#include <mtao/geometry/interpolation/radial_basis_function.hpp>
#include "mtao/overload.hpp"


void Curve::clear() {
    points.clear();
}

mtao::ColVecs2d Curve::V() const {
    return mtao::eigen::stl2eigen(points);
}
mtao::ColVecs2i Curve::E() const {
    mtao::ColVecs2i R(2,edge_size());
    R.row(0) = mtao::RowVecXi::LinSpaced(edge_size(),0,edge_size()-1);
    R.row(1).rightCols(R.cols()-1) = R.row(0).leftCols(R.cols()-1);
    R(1,0) = edge_size()-1;
    return R;
}
mtao::ColVecs2d Curve::area_edge_normals() const {
    mtao::ColVecs2d N(2,edge_size());
    for(size_t idx = 0; idx < edge_size(); ++idx) {
        const mtao::Vec2d& a = points[idx];
        const mtao::Vec2d& b = points[(idx+1)%edge_size()];
        N.col(idx) = mtao::Vec2d(-(b-a).y(),(b-a).x());
    }
    return N;
}
mtao::ColVecs2d Curve::edge_normals() const {
    auto N = area_edge_normals();
    N.colwise().normalize();
    return N;
}


double Curve::winding_number(const mtao::Vec2d& V) const {
    using S = double;
    S value = 0;
    for(size_t i = 0; i < vertex_size(); ++i) {

        auto&& a = points[i] - V;
        auto&& b = points[(i+1)%vertex_size()] - V;

        S aa = std::atan2(a.y(),a.x());
        S ba = std::atan2(b.y(),b.x());
        S ang = ba - aa;
        if(ang > M_PI) {
            ang -= 2 * M_PI;
        } else if(ang <= -M_PI) {
            ang += 2*M_PI;
        }
        value += ang;

    }
    return value / (2 * M_PI);
}
bool Curve::inside(const mtao::Vec2d& V) const{
    return std::abs(winding_number(V)) > 1;

}

auto Curve::evaluator(InterpMode mode) const -> CurveEvaluator {
    switch(mode) {
        case InterpMode::Wachpress: return evaluator(InterpParameters<InterpMode::Wachpress>{});
        case InterpMode::MeanValue: return evaluator(InterpParameters<InterpMode::MeanValue>{});
        case InterpMode::GaussianRBF: return evaluator(InterpParameters<InterpMode::GaussianRBF>{});
        case InterpMode::SplineGaussianRBF: return evaluator(InterpParameters<InterpMode::SplineGaussianRBF>{});
        case InterpMode::DesbrunSplineRBF: return evaluator(InterpParameters<InterpMode::DesbrunSplineRBF>{});
    }
    return evaluator(InterpParameters<InterpMode::Wachpress>{});
}


mtao::VecXd Curve::CurveEvaluator::operator()(const mtao::VecXd& coeffs, const mtao::ColVecs2d& V) const {


    return from_coefficients(coeffs,V);
}
mtao::VecXd Curve::CurveEvaluator::from_coefficients(const mtao::VecXd& coeffs, const mtao::ColVecs2d& V) const {

    mtao::VecXd ret(V.cols());
#if defined(_OPENMP)
#pragma omp parallel for
#endif
    for(int i = 0; i < V.cols(); ++i) {
        mtao::VecXd weights(coeffs.rows());
        std::visit(mtao::overloaded {
                [&](const InterpParameters<InterpMode::Wachpress>&) {
            weights = mtao::geometry::interpolation::wachpress(vertices,V.col(i));
                },
                [&](const InterpParameters<InterpMode::MeanValue>&) {
            weights = mtao::geometry::interpolation::mean_value(vertices,V.col(i));
                },
                [&](const InterpParameters<InterpMode::GaussianRBF>& p) {
            weights = mtao::geometry::interpolation::gaussian_rbf(vertices,V.col(i),p.radius);
                },
                [&](const InterpParameters<InterpMode::SplineGaussianRBF>& p) {
            weights = mtao::geometry::interpolation::spline_gaussian_rbf(vertices,V.col(i),p.radius);
                },
                [&](const InterpParameters<InterpMode::DesbrunSplineRBF>& p) {
            weights = mtao::geometry::interpolation::desbrun_spline_rbf(vertices,V.col(i),p.radius);
                },
                },interp_params);
        ret(i) = coeffs.dot(weights);
    }
    return ret;
}
mtao::ColVecs2d Curve::CurveEvaluator::grad_from_coefficients(const mtao::VecXd& coeffs, const mtao::ColVecs2d& V) const {
    mtao::ColVecs2d L(2,V.cols());
#if defined(_OPENMP)
#pragma omp parallel for
#endif
    for(int i = 0; i < V.cols(); ++i) {
        mtao::ColVecs2d weights(2,coeffs.rows());
        std::visit(mtao::overloaded {
                [&](const InterpParameters<InterpMode::Wachpress>&) {
            weights = mtao::geometry::interpolation::wachpress_gradient(vertices,V.col(i));
                },
                [&](const InterpParameters<InterpMode::MeanValue>&) {
            weights = mtao::geometry::interpolation::mean_value_gradient(vertices,V.col(i));
                },
                [&](const InterpParameters<InterpMode::GaussianRBF>& p) {
            weights = mtao::geometry::interpolation::gaussian_rbg(vertices,V.col(i),p.radius);
                },
                [&](const InterpParameters<InterpMode::SplineGaussianRBF>& p) {
            weights = mtao::geometry::interpolation::spline_gaussian_rbg(vertices,V.col(i),p.radius);
                },
                [&](const InterpParameters<InterpMode::DesbrunSplineRBF>& p) {
            weights = mtao::geometry::interpolation::desbrun_spline_rbg(vertices,V.col(i),p.radius);
                },
                },interp_params);
        L.col(i) = weights.rowwise().sum();
    }
    return L;
}

        
