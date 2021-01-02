#ifndef LEVELSET_DEFORMER_H
#define LEVELSET_DEFORMER_H

#include "levelset.h"
#include "ops.h"
namespace levelset {

template<int _D>
class LevelsetDeformer : public UnaryLevelsetOp<_D> {
  public:
    USE_BASE_LEVELSET_FUNCTION_DEFS(UnaryLevelsetOp)
    using Base::Base;
    using Base::heldValue;


    virtual Vec transform(const constVecRef &v, Scalar t) const {
        return v;
    }
    virtual Vec invtransform(const constVecRef &v, Scalar t) const {
        return transform(v, -t);
    }
    virtual Scalar operator()(const constVecRef &v, Scalar t) const {
        return heldValue(this->transform(v, t), t);
    }
    /*
        virtual Vec dxdt(const constVecRef& v,Scalar t = 0) const {
            return m_held->dxdt(v,t);
        }
        virtual VecVector getSurfacePoints(Scalar t) const {
            auto&& cv = m_held->getSurfacePoints(t);
            std::transform(cv.begin(),cv.end(),cv.begin(),[&](auto&& v) {
                    return this->_invtransform(v,t);
                    });
            return cv;
        }
        */
    /*
        virtual Matrix J(const constVecRef& v, Scalar t) const {
            return Matrix::Identity();
        }
        virtual Vec dfdx(const constVecRef& v,Scalar t = 0, float dx=0.01) const {
            //f(x) = this(held(x))
            //df/dx = dthis/dheld * dheld/dx
            return J(v,t) * m_held->dfdx(this->transform(v,t),t);
        }
        */
    //virtual Scalar dfdt(const constVecRef& v, Scalar t = 0, float dt=0.0) const {
    //    return dfdx(v,t).dot(m_held->dxdt(v,t));
    //}
};

template<int _D>
class LevelsetStaticDeformer : public LevelsetDeformer<_D> {
  public:
    USE_BASE_LEVELSET_FUNCTION_DEFS(LevelsetDeformer)
    using LT = LevelsetDeformer<_D>;
    using LTRawPtr = LevelsetDeformer<_D> *;
    using LTPtr = std::shared_ptr<LT>;
    LevelsetStaticDeformer(const LTPtr &tohold) : Base(tohold) {}

    virtual Vec transform(const constVecRef &v, Scalar t = 0) const {
        return held_lt().transform(v, 1);
    }
    virtual Scalar operator()(const constVecRef &v, Scalar t) const {
        return held_lt().heldValue(this->transform(v, t), t);
    }
    const LT &held_lt() const {
        return *dynamic_cast<const LTRawPtr>(this->held().get());
    }
};

template<int _D>
class LevelsetRangedDeformer : public LevelsetDeformer<_D> {
  public:
    USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)
    using LTPtr = std::shared_ptr<LevelsetDeformer<_D>>;
    using LTRawPtr = LevelsetDeformer<_D> *;
    LevelsetRangedDeformer(const LTPtr &tohold, Scalar start = 0, Scalar end = std::numeric_limits<Scalar>::infinity())
      : Base(tohold), m_start(start), m_range(end - start) {}

    virtual Vec transform(const constVecRef &v, Scalar t) const {
        if (in_range(t)) {
            t -= m_start;
            if (m_range < std::numeric_limits<Scalar>::infinity()) {
                t /= m_range;
            }
            return dynamic_cast<const LTRawPtr>(this->held().get())->transform(v, t);

        } else {
            return v;
        }
    }
    Scalar start() const { return m_start; }
    Scalar range() const { return m_range; }
    Scalar end() const { return m_start + m_range; }
    bool in_range(Scalar t) const {
        t -= m_start;
        return t >= 0 && t <= m_range;
    }

  private:
    Scalar m_start;
    Scalar m_range;
};

template<typename LD>
auto enstatic(const std::shared_ptr<LD> &ptr) {
    constexpr static int D = LD::D;
    return std::make_shared<LevelsetStaticDeformer<D>>(ptr);
}


}// namespace levelset
#endif//LEVELSET_DEFORMER_H
