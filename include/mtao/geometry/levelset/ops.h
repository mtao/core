#ifndef LEVELSET_OPS_H
#define LEVELSET_OPS_H
#include "levelset.h"


namespace levelset {
template<int _D>
class UnaryLevelsetOp : public Levelset<_D> {
  public:
    USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)

    UnaryLevelsetOp(const LSPtr &tohold) : m_held(tohold) {}

    virtual Scalar heldValue(const constVecRef &v, Scalar t) const {
        return (*held())(v, t);
    }
    const LSPtr &held() const { return m_held; }

  protected:
    const LSPtr m_held;
};

template<int _D>
class BinaryLevelsetOp : public Levelset<_D> {
  public:
    USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)

    BinaryLevelsetOp(const LSPtr &a, const LSPtr &b) : m_first(a), m_second(b) {}
    const LSPtr &first() const { return m_first; }
    const LSPtr &second() const { return m_second; }

  protected:
    LSPtr m_first, m_second;
};
template<int _D>
class TernaryLevelsetOp : public Levelset<_D> {
  public:
    USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)

    template<typename... F>
    TernaryLevelsetOp(F... tohold) : m_held{ { tohold... } } {}
    const std::vector<std::shared_ptr<Levelset<D>>> &held() const { return m_held; }
    const std::shared_ptr<Levelset<D>> &held(size_t i) const { return m_held[i]; }
    //TernaryLevelset(F... tohold) {addFuncs(tohold...);}
    //CompositeLevelset(const F& tohold): m_held(Base::upgradeF(tohold)) {}
    /*
        virtual Matrix J(const constVecRef& v, Scalar t) const {
            return Matrix::Identity();
        }
        */
    //template <typename FT>
    //void addFuncs(const FT& ft) {
    //    m_held.push_back(ft);
    //}
    //template <typename FT, typename... Args>
    //    void addFuncs(const FT& ft, Args... args) {
    //        m_held.push_back(ft);
    //        addFuncs(args...);
    //    }
  protected:
    std::vector<std::shared_ptr<Levelset<D>>> m_held;
};
}// namespace levelset
#endif//LEVELSET_OPS_H
