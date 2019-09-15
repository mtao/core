#pragma once
#include <mtao/types.hpp>
#include <Eigen/Sparse>
#include <mtao/iterator/enumerate.hpp>
#include <variant>

class Curve {
    public:
        enum class InterpMode {
            Wachpress, MeanValue, GaussianRBF, SplineGaussianRBF,
            DesbrunSplineRBF
        };
        template <InterpMode>
        struct InterpParameters {};
        class CurveEvaluator;
        mtao::vector<mtao::Vec2d> points;
        void clear() ;
        mtao::ColVecs2d edge_normals() const;
        mtao::ColVecs2d area_edge_normals() const;

        mtao::ColVecs2d V() const;
        mtao::ColVecs2i E() const;





        bool inside(const mtao::Vec2d& V) const;
        double winding_number(const mtao::Vec2d& V) const;

        size_t vertex_size() const { return points.size(); }
        size_t edge_size() const { return points.size(); }

        CurveEvaluator evaluator(InterpMode = InterpMode::MeanValue) const;

        template <InterpMode Mode>
        CurveEvaluator evaluator(const InterpParameters<Mode>& p = {}) const;

};
template <>
struct Curve::InterpParameters<Curve::InterpMode::GaussianRBF> {double radius = 1.;};
template <>
struct Curve::InterpParameters<Curve::InterpMode::SplineGaussianRBF> {double radius = 1.;};
template <>
struct Curve::InterpParameters<Curve::InterpMode::DesbrunSplineRBF> {double radius = 1.;};

class Curve::CurveEvaluator {
    public:
        using PermutationMat = Eigen::PermutationMatrix<Eigen::Dynamic,Eigen::Dynamic, int>;

        template <InterpMode Mode>
        CurveEvaluator(const Curve& c, const InterpParameters<Mode>& params): curve(c), vertices(c.V()), interp_params(params) {}
        mtao::MatXd distances(const mtao::ColVecs2d& Vs) const;

        mtao::VecXd operator()(const mtao::VecXd& f, const mtao::ColVecs2d& V) const;
        mtao::VecXd from_coefficients(const mtao::VecXd& coeffs, const mtao::ColVecs2d& V) const;
        mtao::ColVecs2d grad(const mtao::VecXd& f, const mtao::ColVecs2d& V) const;
        mtao::ColVecs2d grad_from_coefficients(const mtao::VecXd& coeffs, const mtao::ColVecs2d& V) const;

        int size() const { return vertices.cols(); }
        int vertex_size() const { return curve.vertex_size(); }
        auto& V() const { return vertices; }
    private:
        const Curve& curve;

        mtao::ColVecs2d vertices;
        std::variant<
            InterpParameters<InterpMode::MeanValue>
            ,InterpParameters<InterpMode::Wachpress>
            ,InterpParameters<InterpMode::GaussianRBF>
            ,InterpParameters<InterpMode::SplineGaussianRBF>
            ,InterpParameters<InterpMode::DesbrunSplineRBF>
            > interp_params;
};

        template <Curve::InterpMode Mode>
        Curve::CurveEvaluator Curve::evaluator(const InterpParameters<Mode>& p ) const {
            return CurveEvaluator(*this, p);
        }
