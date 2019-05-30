#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include "levelset.h"
namespace levelset {
template <int _D>
class SphereLevelset: public Levelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)
            template <typename VecType>
            SphereLevelset(Scalar r = Scalar(1.0)): m_center(Vec::Zero()), m_radius(r) {
            }
        template <typename VecType>
            SphereLevelset(const VecType& c, Scalar r): m_center(c), m_radius(r) {
            }
            Scalar operator()(const constVecRef& v, Scalar t) const {
                return (v - m_center).norm() - m_radius;
            }
            /*
        virtual Vec dfdx(const constVecRef& v,Scalar t = 0, float dx=0.01) const {
            return (v - m_center).normalized();
        }
        */
        const Vec& center() const {return m_center;}
        Scalar radius() const { return m_radius;}
    private:
        Vec m_center;
        Scalar m_radius;

};
template <int _D>
class CubeLevelset: public Levelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)
            CubeLevelset(Scalar r = Scalar(1.0)): m_center(Vec::Zero()), m_radius(r) {
            }
            template <typename VecType>
            CubeLevelset(const VecType& c, Scalar r): m_center(c), m_radius(r) {
            }
            Scalar operator()(const constVecRef& v, Scalar t) const {
                Scalar mindist = std::numeric_limits<Scalar>::max();;
                Vec r = v - m_center;
                Scalar inside = (r.template lpNorm<Eigen::Infinity>() < m_radius)?-1:1;
                for(int d = 0; d < _D; ++d) {
                    auto&& p = r(d);
                    r(d) = std::abs(v(d)) - m_radius;
                }
                return std::min(r.maxCoeff(),inside * r.norm());
            }
            /*
        virtual Vec dfdx(const constVecRef& v,Scalar t = 0, float dx=0.01) const {
            return (v - m_center).normalized();
        }
        */
        const Vec& center() const {return m_center;}
        Scalar radius() const { return m_radius;}
    private:
        Vec m_center;
        Scalar m_radius;

};
    template <int D, typename Scalar = typename Levelset<D>::Scalar, typename Vec = typename Levelset<D>::Vec>
        auto sphere(const Vec& center = Vec::Zero(), Scalar radius = Scalar(1)) {
            return std::make_shared<SphereLevelset<D>>(center,radius);
        }
    template <int D, typename Scalar = typename Levelset<D>::Scalar, typename Vec = typename Levelset<D>::Vec>
        auto cube(const Vec& center = Vec::Zero(), Scalar radius = Scalar(1)) {
            return std::make_shared<CubeLevelset<D>>(center,radius);
        }

/*
template <int _D>
class BlobbyLevelset: public Levelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)
            template <typename VecType>
            BlobbyLevelset(const VecType& c, Scalar r): m_center(c), m_radius(r) {
            }
            Scalar operator()(const constVecRef& v, Scalar t) const {
                return (v - m_center).norm();
                return radial((v - m_center).norm());
            }
            Scalar dradial(Scalar r) const {
                auto p = r/m_radius;
                if(p>=1) {
                    return 0;
                } else {
                    return 6*std::pow<Scalar>((1-p*p),2)*p/m_radius;
                }
            }
            Scalar radial(Scalar r) const {
                auto p = r/m_radius;
                if(p>=1) {
                    return 0;
                } else {
                    return -std::pow<Scalar>((1-p*p),3);
                }
            }
        virtual Vec dfdx(const constVecRef& v,Scalar t = 0, float dx=0.01) const {
            return dradial((v-m_center).norm())*(v - m_center).normalized();
        }
    private:
        Vec m_center;
        Scalar m_radius;

};
*/
}
#endif//PRIMITIVES_H
