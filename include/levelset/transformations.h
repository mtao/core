#ifndef LEVELSET_TRANSFORMATIONS_H
#define LEVELSET_TRANSFORMATIONS_H
#include "deformer.h"

namespace levelset {

template <int _D>
class LevelsetTranslator: public LevelsetDeformer<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(LevelsetDeformer)
            template <typename Func, typename VecType>
            LevelsetTranslator(const Func& f, const VecType& vel): Base(f) , m_velocity(vel) {}
        virtual Vec transform(const constVecRef& v, Scalar t) const {
            return v - m_velocity * t;
        }
    private:
        Vec m_velocity;


};


template <int _D>
class LevelsetRotator;

template <>
class LevelsetRotator<2>: public LevelsetDeformer<2> {
    public:
        static constexpr int _D = 2;
        USE_BASE_LEVELSET_FUNCTION_DEFS(LevelsetDeformer)
            template <typename Func, typename VecType>
            LevelsetRotator(const Func& f, const VecType& c,Scalar angvel): Base(f) , m_center(c), m_angvel(angvel) {}
            template <typename Func, typename VecType, typename VecType2>
            LevelsetRotator(const Func& f, const VecType& c, const VecType2& axis, Scalar angvel): Base(f) , m_center(c), m_angvel(angvel) {}
        /*
        virtual Vec dxdt(const constVecRef& v,Scalar t = 0) const {
            auto&& p = v - m_center;
            return m_angvel * Vec(p.y(),-p.x()) + Base::dxdt(v,t);
        }
        */
        virtual Vec transform(const constVecRef& v, Scalar t) const {
            return m_center + Eigen::Rotation2D<Scalar>(t*m_angvel) * (v-m_center);
        }
        /*
        //TODO: This is incorrect :(
        virtual Matrix J(const constVecRef& v, Scalar t) const {
            //return Matrix::Identity();
            auto&& vc = v - m_center;
            auto&& r = Eigen::Rotation2D<Scalar>(t*m_angvel + 2*std::atan(1));
            r = Eigen::Rotation2D<Scalar>(-t*m_angvel);
            Scalar scale = m_angvel / (2 * std::atan(1));
            scale = 1;
            return scale * r.matrix();

        }
        */
    private:
        Vec m_center;
        Scalar m_angvel;


};

template <>
class LevelsetRotator<3>: public LevelsetDeformer<3> {
    public:
        static constexpr int _D = 3;
        USE_BASE_LEVELSET_FUNCTION_DEFS(LevelsetDeformer)
            using AngleAxis = Eigen::AngleAxis<Scalar>;
            template <typename Func, typename VecType, typename VecType2>
            LevelsetRotator(const Func& f, const VecType& c, const VecType2& axis, Scalar angvel): Base(f) , m_center(c), m_axis(axis), m_angvel(angvel) {}
            /*
        virtual Vec dxdt(const constVecRef& v,Scalar t = 0) const {
            auto&& p = v - m_center;
            return p.cross(m_axis);
        }
        */
        virtual Vec transform(const constVecRef& v, Scalar t) const {
            auto&& vc = v - m_center;
            return m_center + AngleAxis(m_angvel * t,m_axis) * (v-m_center);
        }
        /*
        //TODO: This is incorrect :(
        virtual Matrix J(const constVecRef& v, Scalar t) const {
            //return Matrix::Identity();
            auto&& vc = v - m_center;
            auto&& r = AngleAxis(-m_angvel * t,m_axis);
            Scalar scale = m_angvel / (2 * std::atan(1));
            scale = 1;
            return scale * r.matrix();

        }
        */
    private:
        Vec m_center;
        Vec m_axis;
        Scalar m_angvel;


};

template <int _D>
class LevelsetScaler: public LevelsetDeformer<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(LevelsetDeformer)
            template <typename Func, typename VecType, typename VecType2>
            LevelsetScaler(const Func& f, const VecType& c, const VecType2& scale = Vec::Ones()): Base(f) ,m_center(c), m_scale(scale) {}
        /*
        virtual Vec dxdt(const constVecRef& v,Scalar t = 0) const {
            return Base::dxdt(v,t);
        }
        */
        virtual Vec transform(const constVecRef& v, Scalar t) const {
            auto cv = v - m_center;
            //auto scv = m_scale.cwiseProduct(cv);
            auto scv = cv.array() * (t * (m_scale.array() - 1)+1);

            return scv.matrix() + m_center;
        }
        /*
        virtual Vec _invtransform(const constVecRef& v, Scalar t) const {
            auto cv = v - m_center;
            //auto scv = cv.cwiseQuotient(m_scale);
            auto scv = cv.cwiseProduct(m_scale);
            //auto scv = cv;

            //return scv+m_center;
            return scv + m_center;
        }
        */
    private:
        Vec m_center;
        Vec m_scale;


};

    template <int D, typename Vec = typename Levelset<D>::Vec>
        auto translation(const typename Levelset<D>::Ptr& f, const Vec& dir) {
            return std::make_shared<LevelsetTranslator<D>>(f,dir);
        }
    template <int D, typename Vec = typename Levelset<D>::Vec>
        auto axis_translation(const typename Levelset<D>::Ptr& f, int Dim) {
            return std::make_shared<LevelsetTranslator<D>>(f,Vec::Unit(Dim));
        }
    template <int D, typename Vec = typename Levelset<D>::Vec>
        auto rotation(const typename Levelset<D>::Ptr& f, const Vec& center = Vec::Zero(), const Vec& axis = Vec::Unit(D-1)) {
            return std::make_shared<LevelsetRotator<D>>(f,center,axis.normalized(),axis.norm());
        }

    template <int D, typename Vec = typename Levelset<D>::Vec>
        auto scaling(const typename Levelset<D>::Ptr& f, const Vec& center = Vec::Zero(), const Vec& s = Vec::Ones()) {
            return std::make_shared<LevelsetScaler<D>>(f,center,s);
        }

}


#endif//LEVELSET_TRANSFORMATIONS_H
