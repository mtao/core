#ifndef LEVELSET_TRANSFORMER_H
#define LEVELSET_TRANSFORMER_H

template <int _D>
class LevelsetTransformer: public Levelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)

        template <typename F>
        LevelsetTransformer(const F& tohold): m_held(tohold) {}

        virtual Vec transform(const constVecRef& v, Scalar t) const {
            return v;
        }
        virtual Vec invtransform(const constVecRef& v, Scalar t) const {
            return transform(v,-t);
        }
        virtual Scalar operator()(const constVecRef& v, Scalar t) const {

            return heldValue(this->transform(v,t),t);
        }
        virtual Scalar heldValue(const constVecRef& v, Scalar t) const {
            return held()(v,t);
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
        const std::shared_ptr<Levelset<D> >& held() const {return m_held;}
    protected:
        const std::shared_ptr<Levelset<D> > m_held;
};

template <int _D>
class LevelsetStaticTransformer: public Levelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)
            using LTPtr = std::shared_ptr<LevelsetTransformer<_D>>;

        LevelsetStaticTransformer(const LTPtr& tohold): m_held(tohold) {}

        virtual Vec transform(const constVecRef& v, Scalar t) const {
            held()->transform(v,1);
        }
        virtual Vec invtransform(const constVecRef& v, Scalar t) const {
            return transform(v,-t);
        }
        virtual Scalar operator()(const constVecRef& v, Scalar t) const {

            return held()->heldValue(this->transform(v,t),t);
        }
        virtual Scalar heldValue(const constVecRef& v, Scalar t) const {
            return held()->held()(v,t);
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
        const LTPtr& held() const {return m_held;}
    protected:
        const LTPtr m_held;
};


#endif//LEVELSET_TRANSFORMER_H
