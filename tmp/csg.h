template <int _D>
class BinaryLevelsetOp: public ScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(ScalarFunction)

        template <typename... F>
        BinaryLevelsetOp(const Base::Ptr& a, const Base::Ptr& b): m_first(a), m_second(b) {}
        virtual Matrix J(const constVecRef& v, Scalar t) const {
            return Matrix::Identity();
        }
    protected:
        Base::Ptr m_first,m_second;
};
template <int _D>
class CSGIntersection: public BinaryLevelsetOp<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(BinaryLevelsetOp)
        CSGIntersection(const Base::Ptr& a, const Base::Ptr& b): Base(a,b) {}
        void operator()(const constVecRef& v)  const {
            return std::max((*m_first)(v),(*m_second)(v));
        }
};
template <int _D>
class CSGUnion: public BinaryLevelsetOp<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(BinaryLevelsetOp)
        CSGUnion(const Base::Ptr& a, const Base::Ptr& b): Base(a,b) {}
        void operator()(const constVecRef& v)  const {
            return std::min((*m_first)(v),(*m_second)(v));
        }
};
template <int _D>
class CSGDifference: public CSGUnion<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(CSGUnion)
        CSGDifference(const Base::Ptr& a, const Base::Ptr& b): Base(a,CSGInverse<_D>(b)) {}
};

template <int _D>
class CompositeScalarFunction: public ScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(ScalarFunction)

        template <typename... F>
        CompositeScalarFunction(F... tohold) {addFuncs(tohold...);}
        //CompositeScalarFunction(const F& tohold): m_held(Base::upgradeF(tohold)) {}
        virtual Matrix J(const constVecRef& v, Scalar t) const {
            return Matrix::Identity();
        }
        template <typename FT>
        void addFuncs(const FT& ft) {
            m_held.push_back(ft);
        }
        template <typename FT, typename... Args>
            void addFuncs(const FT& ft, Args... args) {
                m_held.push_back(ft);
                addFuncs(args...);
            }
    protected:
        std::vector<std::shared_ptr<ScalarFunction<D> > > m_held;
};
template <int _D>
class SumScalarFunction: public CompositeScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(CompositeScalarFunction)
            template <typename... Args>
            SumScalarFunction(Args... args): Base(args...) {}
        virtual Scalar operator()(const constVecRef& v, Scalar t) const {
            Scalar val = 0;
            for(auto&& f: Base::m_held) {
                val += f->operator()(v,t);
            }
            return val+.1;
        }
    private:


};


template <int _D>
class MinScalarFunction: public CompositeScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(CompositeScalarFunction)
            template <typename... Args>
            MinScalarFunction(Args... args): Base(args...) {}
        virtual Scalar operator()(const constVecRef& v, Scalar t) const {
            Scalar min = std::numeric_limits<Scalar>::max();
            for(auto&& f: Base::m_held) {
                min = std::min(min,f->operator()(v,t));
            }
            return min;
        }
    private:


};

