#ifndef LEVELSET_FUNC_H
#define LEVELSET_FUNC_H
#include <iostream>
#include <Eigen/Dense>
#include <memory>
#include <vector>
//#include "mesh.h"





template <int _D>
class Levelset {
    public:
        static constexpr int D = _D;
        using Scalar = float;
        using Ptr = std::shared_ptr<Levelset<D>>;

        using Vec = Eigen::Matrix<Scalar,D,1>;
        using Matrix = Eigen::Matrix<Scalar,D,D>;
        using VecRef = Eigen::Ref<Vec>;
        using constVecRef = Eigen::Ref<const Vec>;
        using VecVector = std::vector<Vec,Eigen::aligned_allocator<Vec> >;
//        virtual Scalar operator()(const constVecRef& v) const ;
        virtual Scalar operator()(const constVecRef& v,Scalar t = 0) const = 0;
        /*
        
        virtual Vec transform(const constVecRef& v, Scalar t = 0) const {
            return v;
        }
        virtual Vec invtransform(const constVecRef& v, Scalar t) const {
            return this->transform(v,-t);
        }
        */
        /*
        virtual Vec dfdx(const constVecRef& v,Scalar t = 0, Scalar dx=0.01) const {
            auto&& me = *this;
            Vec ret;
            Vec p2;
            for(int d = 0; d < D; ++d) {
                p2 = v;
                p2(d) += dx;
                ret(d) = (me(p2,t) - me(v,t))/dx;
            }
            //std::swap(ret.x(),ret.y());
            //ret.y() = -ret.y();
            return ret;
        }
        */
        /*
        virtual Matrix dudx(const constVecRef& v,Scalar t = 0, Scalar dx=0.01) const {
            Matrix ret;
            Vec p2;
            for(int d = 0; d < D; ++d) {
                p2 = v;
                p2(d) += dx;
                auto l = dxdt(p2,t);
                p2(d) -= 2*dx;
                auto r = dxdt(p2,t);
                ret.col(d) = (l-r)/(2*dx);
            }
            //std::swap(ret.x(),ret.y());
            //ret.y() = -ret.y();
            return ret;
        }
        */
        /*
        Scalar dfdt(const constVecRef& v,Scalar t = 0, Scalar dt=0.001) const {
            auto&& me = *this;
            return (me(v,t+dt)-me(v,t-dt))/(2*dt);
        }
        virtual Vec dxdt(const constVecRef& v,Scalar t = 0) const {
            return Vec::Zero();
        }
        */
    private:
};



#define USE_BASE_LEVELSET_FUNCTION_DEFS(BASE) \
    static constexpr int D = _D;\
    using Base = BASE<D>;\
    using BasePtr = typename Base::Ptr;
    using ZeroFunc = ZeroLevelset<D>;\
    using Scalar = typename Base::Scalar;\
    using Vec = typename Base::Vec;\
    using Matrix = typename Base::Matrix;\
    using VecRef = typename Base::VecRef;\
    using constVecRef = typename Base::constVecRef;\
    using VecVector = typename Base::VecVector;

template <int _D>
class ZeroLevelset: public Levelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)
        Scalar operator()(const constVecRef& v, Scalar t = 0) const {
            return Scalar(0);
        }
        static std::shared_ptr<ZeroFunc> ptr() { return std::make_shared<ZeroLevelset<D>>();}
};

#endif//LEVELSET_FUNC_H
