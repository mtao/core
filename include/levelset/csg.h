template <int _D>
class BinaryLevelsetOp: public Levelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)

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
        USE_BASE_LEVELSET_FUNCTION_DEFS(BinaryLevelsetOp)
        CSGIntersection(const Base::Ptr& a, const Base::Ptr& b): Base(a,b) {}
        void operator()(const constVecRef& v)  const {
            return std::max((*m_first)(v),(*m_second)(v));
        }
};
template <int _D>
class CSGUnion: public BinaryLevelsetOp<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(BinaryLevelsetOp)
        CSGUnion(const Base::Ptr& a, const Base::Ptr& b): Base(a,b) {}
        void operator()(const constVecRef& v)  const {
            return std::min((*m_first)(v),(*m_second)(v));
        }
};
template <int _D>
class CSGDifference: public CSGUnion<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(CSGUnion)
        CSGDifference(const Base::Ptr& a, const Base::Ptr& b): Base(a,CSGInverse<_D>(b)) {}
};

template <int _D>
class CompositeLevelset: public Levelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)

        template <typename... F>
        CompositeLevelset(F... tohold) {addFuncs(tohold...);}
        //CompositeLevelset(const F& tohold): m_held(Base::upgradeF(tohold)) {}
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
        std::vector<std::shared_ptr<Levelset<D> > > m_held;
};
template <int _D>
class SumLevelset: public CompositeLevelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(CompositeLevelset)
            template <typename... Args>
            SumLevelset(Args... args): Base(args...) {}
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
class MinLevelset: public CompositeLevelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(CompositeLevelset)
            template <typename... Args>
            MinLevelset(Args... args): Base(args...) {}
        virtual Scalar operator()(const constVecRef& v, Scalar t) const {
            Scalar min = std::numeric_limits<Scalar>::max();
            for(auto&& f: Base::m_held) {
                min = std::min(min,f->operator()(v,t));
            }
            return min;
        }
    private:


};

