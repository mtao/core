template <int _D>
class TranslateScalarFunction: public HoldingScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(HoldingScalarFunction)
            template <typename Func, typename VecType>
            TranslateScalarFunction(const Func& f, const VecType& vel): Base(f) , m_velocity(vel) {}
        virtual Vec dxdt(const constVecRef& v,Scalar t = 0) const {
            return Base::dxdt(v,t);
        }
        virtual Vec _transform(const constVecRef& v, Scalar t) const {
            return v - m_velocity ;
        }
        virtual Vec _invtransform(const constVecRef& v, Scalar t) const {
            return v + m_velocity ;
        }
    private:
        Vec m_velocity;


};

template <int _D>
class TranslatingScalarFunction: public HoldingScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(HoldingScalarFunction)
            template <typename Func, typename VecType>
            TranslatingScalarFunction(const Func& f, const VecType& vel): Base(f) , m_velocity(vel) {}
        virtual Vec dxdt(const constVecRef& v,Scalar t = 0) const {
            return m_velocity + Base::dxdt(v,t);
        }
        virtual Vec _transform(const constVecRef& v, Scalar t) const {
            return v - m_velocity * t;
        }
    private:
        Vec m_velocity;


};

template <int _D>
class RotatingScalarFunction;

template <>
class RotatingScalarFunction<2>: public HoldingScalarFunction<2> {
    public:
        static constexpr int _D = 2;
        USE_BASE_SCALAR_FUNCTION_DEFS(HoldingScalarFunction)
            template <typename Func, typename VecType>
            RotatingScalarFunction(const Func& f, const VecType& c,Scalar angvel): Base(f) , m_center(c), m_angvel(angvel) {}
        virtual Vec dxdt(const constVecRef& v,Scalar t = 0) const {
            auto&& p = v - m_center;
            return m_angvel * Vec(p.y(),-p.x()) + Base::dxdt(v,t);
        }
        virtual Vec _transform(const constVecRef& v, Scalar t) const {
            auto&& vc = v - m_center;
            auto&& rv = Eigen::Rotation2D<Scalar>(t*m_angvel) * vc + m_center;
            return rv;
        }
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
    private:
        Vec m_center;
        Scalar m_angvel;


};

template <>
class RotatingScalarFunction<3>: public HoldingScalarFunction<3> {
    public:
        static constexpr int _D = 3;
        USE_BASE_SCALAR_FUNCTION_DEFS(HoldingScalarFunction)
            using AngleAxis = Eigen::AngleAxis<Scalar>;
            template <typename Func, typename VecType, typename VecType2>
            RotatingScalarFunction(const Func& f, const VecType& c, const VecType2& axis, Scalar angvel): Base(f) , m_center(c), m_axis(axis), m_angvel(angvel) {}
        virtual Vec dxdt(const constVecRef& v,Scalar t = 0) const {
            auto&& p = v - m_center;
            return p.cross(m_axis);
        }
        virtual Vec _transform(const constVecRef& v, Scalar t) const {
            auto&& vc = v - m_center;
            auto&& rv = AngleAxis(m_angvel * t,m_axis) * vc + m_center;
            return rv;
        }
        //TODO: This is incorrect :(
        virtual Matrix J(const constVecRef& v, Scalar t) const {
            //return Matrix::Identity();
            auto&& vc = v - m_center;
            auto&& r = AngleAxis(-m_angvel * t,m_axis);
            Scalar scale = m_angvel / (2 * std::atan(1));
            scale = 1;
            return scale * r.matrix();

        }
    private:
        Vec m_center;
        Vec m_axis;
        Scalar m_angvel;


};

template <int _D>
class ScalingScalarFunction: public HoldingScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(HoldingScalarFunction)
            template <typename Func, typename VecType, typename VecType2>
            ScalingScalarFunction(const Func& f, const VecType& c, const VecType2& scale = Vec::Ones()): Base(f) ,m_center(c), m_scale(scale) {}
        virtual Vec dxdt(const constVecRef& v,Scalar t = 0) const {
            return Base::dxdt(v,t);
        }
        virtual Vec _transform(const constVecRef& v, Scalar t) const {
            auto cv = v - m_center;
            //auto scv = m_scale.cwiseProduct(cv);
            auto scv = cv.cwiseQuotient(m_scale);

            return scv + m_center;
        }
        virtual Vec _invtransform(const constVecRef& v, Scalar t) const {
            auto cv = v - m_center;
            //auto scv = cv.cwiseQuotient(m_scale);
            auto scv = cv.cwiseProduct(m_scale);
            //auto scv = cv;

            //return scv+m_center;
            return scv + m_center;
        }
    private:
        Vec m_center;
        Vec m_scale;


};
