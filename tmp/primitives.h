template <int _D>
class SphereLevelset: public ScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(ScalarFunctionWithId)
            template <typename VecType>
            SphereLevelset(const VecType& c, Scalar r): m_center(c), m_radius(r) {
            }
            Scalar operator()(const constVecRef& v, Scalar t) const {
                return (v - m_center).norm() - m_radius;
            }
        virtual Vec dfdx(const constVecRef& v,Scalar t = 0, float dx=0.01) const {
            return (v - m_center).normalized();
        }
        const Vec& center() const {return m_center;}
        Scalar radius() const { return m_radius;}
    private:
        Vec m_center;
        Scalar m_radius;

};

template <int _D>
class BlobbyLevelset: public ScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(ScalarFunctionWithId)
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
