#ifndef LEVELSET_CSG_H
#define LEVELSET_CSG_H
#include "levelset.h"
#include "ops.h"

namespace levelset {
template <int _D>
class CSGInverse: public UnaryLevelsetOp<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(UnaryLevelsetOp)
        using Base::Base;
        using Base::held;
        Scalar operator()(const constVecRef& v, Scalar t)  const {
            return -(*held())(v,t);
        }
};
template <int _D>
class CSGIntersection: public BinaryLevelsetOp<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(BinaryLevelsetOp)
        using Base::first;
        using Base::second;
        CSGIntersection(const LSPtr& a, const LSPtr& b): Base(a,b) {}
        Scalar operator()(const constVecRef& v, Scalar t)  const {
            return std::max((*first())(v,t),(*second())(v,t));
        }
};
template <int _D>
class CSGUnion: public BinaryLevelsetOp<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(BinaryLevelsetOp)
        using Base::first;
        using Base::second;
        CSGUnion(const LSPtr& a, const LSPtr& b): Base(a,b) {}
        Scalar operator()(const constVecRef& v,Scalar t)  const {
            return std::min((*first())(v,t),(*second())(v,t));
        }
};
template <int _D>
class CSGDifference: public CSGUnion<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(CSGUnion)
            using Base::Base;
        using Base::first;
        using Base::second;
        //CSGDifference(const LSPtr& a, const LSPtr& b): Base(a,CSGInverse<_D>(b)) {}
        Scalar operator()(const constVecRef& v, Scalar t)  const {
            return std::min((*first())(v,t),-(*second())(v,t));
        }
};

template <int _D>
class SumLevelset: public TernaryLevelsetOp<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(TernaryLevelsetOp)
            using Base::Base;
        virtual Scalar operator()(const constVecRef& v, Scalar t) const {
            Scalar val = 0;
            for(auto&& f: Base::m_held) {
                val += f->operator()(v,t);
            }
            return val+.1;
        }
};


template <int _D>
class TernaryCSGUnion: public TernaryLevelsetOp<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(TernaryLevelsetOp)
        using Base::Base;
        using Base::held;
        virtual Scalar operator()(const constVecRef& v, Scalar t) const {
            //Scalar min = std::numeric_limits<Scalar>::max();
            return std::min(held().begin(),held().end(),[&](auto&& f) {
                    return f->operator()(v,t);
                    });
        }
};
template <int _D>
class TernaryCSGIntersection: public TernaryLevelsetOp<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(TernaryLevelsetOp)
        using Base::Base;
        using Base::held;
        virtual Scalar operator()(const constVecRef& v, Scalar t) const {
            //Scalar min = std::numeric_limits<Scalar>::max();
            return std::max(held().begin(),held().end(),[&](auto&& f) {
                    return f->operator()(v,t);
                    });
        }
};

template <int D, typename A>
auto csg_inverse(const A& a) {
    return std::make_shared<CSGInverse<D>>(a);
}
template <int D, typename A, typename B>
auto csg_intersection(const A& a, const B& b) {
    return std::make_shared<CSGIntersection<D>>(a,b);
}
template <int D, typename A, typename B>
auto csg_union(const A& a, const B& b) {
    return std::make_shared<CSGUnion<D>>(a,b);
}
template <int D, typename A, typename B>
auto csg_difference(const A& a, const B& b) {
    return std::make_shared<CSGDifference<D>>(a,b);
}
template <int D, typename... A>
auto csg_multi_intersection(const A... a) {
    return std::make_shared<TernaryCSGIntersection<D>>(a...);
}
template <int D, typename... A>
auto csg_multi_union(const A... a) {
    return std::make_shared<TernaryCSGUnion<D>>(a...);
}

}

#endif//LEVELSET_CSG_H
