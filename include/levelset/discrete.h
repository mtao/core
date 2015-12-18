#ifndef DISCRETE_SCALAR_FUNC_H
#define DISCRETE_SCALAR_FUNC_H
#include "levelset.h"
#include "../grid/grid.h"

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
            void load(const typename Base::Ptr& f, Scalar start=0, Scalar end=1, size_t slices=50);
            void load(GridVector&& gv, Scalar start=0, Scalar end=1);//To reduce copying a lways use rvalue ref
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
    void DenseDiscreteLevelset<_D>::load(const typename Base::Ptr& f, Scalar start, Scalar end, size_t slices) {
        m_dt = (end-start)/slices;
        m_dt = 1e-2;
        m_start = start;
        m_end = end;
        using MapType = Eigen::Map<const Eigen::Matrix<typename IndexType::value_type,D,1>>;
        auto index2world = [&](auto&& index) -> auto {
            return m_dx.cwiseProduct(MapType(index.begin()).template cast<Scalar>());
        };
        m_grids.resize(slices,m_nx);
        for(size_t i = 0; i < slices; ++i) {
            auto&& g = m_grids[i];
            Scalar t = start + m_dt * i;
            g.loop_write_idx([&](auto&& idx){
                    return (*f)(index2world(idx),t);
                    });
        }
        //std::cout << "Grid sizes: ";
        //for(auto&& g: m_grids) {
        //    std::cout << "(";
        //    for(auto&& d: g.shape()) {
        //        std::cout << d << ", ";
        //    }
        //    std::cout << ": " << g.size() << ")";
        //}
        //std::cout << std::endl;
    }
    template <int _D>
    void DenseDiscreteLevelset<_D>::load(GridVector&& g, Scalar start, Scalar end) {
        size_t slices = g.size();
        m_dt = (end-start)/slices;
        m_start = start;
        m_end = end;
        m_grids = std::move(g);
    }
}
#endif//DISCRETE_SCALAR_FUNC_H
