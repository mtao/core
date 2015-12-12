#ifndef LEVELSET_COMPOSITION_H
#define LEVELSET_COMPOSITION_H

template <int _D>
class HoldingScalarFunction: public ScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(ScalarFunction)

        template <typename F>
        HoldingScalarFunction(const F& tohold): m_held(tohold) {}
        //HoldingScalarFunction(const F& tohold): m_held(Base::upgradeF(tohold)) {}
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
        virtual Vec transform(const constVecRef& v, Scalar t) const {
            auto&& p = _transform(v,t);
            return m_held->transform(p,t);
        }
        virtual Vec invtransform(const constVecRef& v, Scalar t) const {

            return _invtransform(m_held->invtransform(v,t),t);
        }
        virtual Vec _transform(const constVecRef& v, Scalar t) const {
            return v;
        }
        virtual Vec _invtransform(const constVecRef& v, Scalar t) const {
            return _transform(v,-t);
        }
        virtual Scalar operator()(const constVecRef& v, Scalar t) const {

                auto  p = this->_transform(v,t);
                //p = v;

            return m_held->operator()(p,t);
        }
        virtual Matrix J(const constVecRef& v, Scalar t) const {
            return Matrix::Identity();
        }
        virtual Vec dfdx(const constVecRef& v,Scalar t = 0, float dx=0.01) const {
            //f(x) = this(held(x))
            //df/dx = dthis/dheld * dheld/dx
            return J(v,t) * m_held->dfdx(this->transform(v,t),t);
        }
        //virtual Scalar dfdt(const constVecRef& v, Scalar t = 0, float dt=0.0) const {
        //    return dfdx(v,t).dot(m_held->dxdt(v,t));
        //}
    protected:
        const std::shared_ptr<ScalarFunction<D> > m_held;
        static int id_counter;
        int id = id_counter;
};
template <int _D>
int HoldingScalarFunction<_D>::id_counter = 0;



#endif//LEVELSET_COMPOSITION_H
