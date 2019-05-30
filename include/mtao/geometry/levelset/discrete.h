#ifndef DISCRETE_SCALAR_FUNC_H
#define DISCRETE_SCALAR_FUNC_H
#include "levelset.h"
#include "../grid/grid.h"
#include <iostream>

namespace levelset {

    template <int _D>
    class DenseDiscreteLevelset: public Levelset<_D> {
        public:
            USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)
    
            using Ptr = std::shared_ptr<DenseDiscreteLevelset<D>>;
            using GridType = typename mtao::Grid<Scalar,D>;
            using BBox = typename Eigen::AlignedBox<Scalar,D>;
            using IndexType = typename GridType::index_type;
            using GridVector = typename std::vector<GridType>;
            using iterator = typename GridVector::iterator;
            iterator begin() {return m_grids.begin();}
            iterator end() {return m_grids.end();}
            const GridType& front() {return m_grids.front();}
            const GridType& back() {return m_grids.back();}
            DenseDiscreteLevelset(const BBox& bb, const IndexType& nx): m_bbox(bb), m_nx(nx) {
                auto range = bb.sizes();
                for(int i =0 ; i < D; ++i) {
                    m_dx[i] = range[i] / (nx[i]-1);
                }
            }
            void load(const typename Base::Ptr& f, size_t slices=50, Scalar duration=1.0);
            Scalar operator()(const constVecRef& v, Scalar t = 0) const {
                Vec v2 = m_dx.cwiseProduct(v);//Put into unit cube
                if(t <= 0){
                    return mtao::lerp(m_grids.front(),v);
                } else if(t >= 1) {
                    return mtao::lerp(m_grids.back(),v);
                } else {
                    Scalar p = t * m_grids.size();
                    size_t idx = std::floor(p);
                    auto a = mtao::lerp(m_grids[idx  ],v);
                    auto b = mtao::lerp(m_grids[idx+1],v);
                    Scalar alpha = p - idx;
                    return mtao::interp_internal::lerp(a,b,alpha);
                }
            }
            const GridType& operator()(size_t f) const {return m_grids[f];}
            GridType& operator()(size_t f) {return m_grids[f];}
            Scalar dt() const { return m_dt;}
            size_t size() const { return m_grids.size(); }
            const IndexType& shape() const {return m_nx;}
            IndexType& shape() {return m_nx;}
            GridVector& grids() {return m_grids;}
            const GridVector& grids() const {return m_grids;}
        private:
            BBox m_bbox;
            IndexType m_nx;
            Vec m_dx;
            Scalar m_dt = Scalar(1e-2);
            GridVector m_grids;
    
    
    };
    
    template <int _D>
    void DenseDiscreteLevelset<_D>::load(const typename Base::Ptr& f, size_t slices, Scalar duration) {
        m_dt = duration/slices;
        using MapType = Eigen::Map<const Eigen::Matrix<typename IndexType::value_type,D,1>>;
        auto index2world = [&](auto&& index) -> auto {
            return m_dx.cwiseProduct(MapType(index.begin()).template cast<Scalar>()) + m_bbox.min();
        };
        m_grids.resize(slices,m_nx);
        for(size_t i = 0; i < slices; ++i) {
            auto&& g = m_grids[i];
            Scalar t = m_dt * i;
            g.loop_write_idx([&](auto&& idx){
                    return (*f)(index2world(idx),t);
                    });
        }
    }
    namespace internal {
        template <int D>
        constexpr mtao::compat::array<int,D> const_index(int N) {
            mtao::compat::array<int,D> arr;
            arr.fill(N);
            return arr;
        }
    }
    template <int D, typename LSPtr, class DDL = DenseDiscreteLevelset<D>, typename BBox = typename DDL::BBox, typename IndexType = typename DDL::IndexType, typename VecType = typename DDL::Vec>
    auto dense_discrete(const LSPtr& ptr, int nframes = 10, float duration=1.0, const BBox& bb = BBox(-VecType::Ones(),VecType::Ones()), const IndexType& nx = internal::const_index<D>(50)) {

        auto ret = std::make_shared<DDL>(bb,nx);
        ret->load(ptr,nframes,duration);
        return ret;

    }
}
#endif//DISCRETE_SCALAR_FUNC_H
