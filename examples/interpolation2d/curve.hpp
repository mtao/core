#pragma once
#include <mtao/types.hpp>
#include <Eigen/Sparse>
#include <mtao/iterator/enumerate.hpp>

class Curve {
    public:
        enum class InterpMode {
            Wachpress, MeanValue
        };
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

};

class Curve::CurveEvaluator {
    public:
        using PermutationMat = Eigen::PermutationMatrix<Eigen::Dynamic,Eigen::Dynamic, int>;
        CurveEvaluator(const Curve& c, InterpMode);
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
        InterpMode interp_mode;
};

