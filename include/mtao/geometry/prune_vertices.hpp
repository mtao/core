#pragma once
#include "mtao/geometry/kdtree.hpp"
#include <map>
#include "mtao/iterator/enumerate.hpp"
#include "mtao/eigen/stl2eigen.hpp"

namespace mtao { namespace geometry {
    //expects a mtao::vector<mtao::Vector<T,D>>
    template <typename Container 
        , typename T
        >
        auto prune(const Container& V, T eps = T(1e-8)) -> std::tuple<Container, std::map<int,int>> {
            using Vec = typename Container::value_type;
            constexpr static int D = Vec::RowsAtCompileTime;
            using Scalar = typename Vec::Scalar;
            std::map<int,int> remap;
            Container ret_vec;
            if(eps == 0 && V.size() > 0) {
                constexpr static bool static_size = D != Eigen::Dynamic;
                using StlVec= std::conditional_t<static_size,std::array<Scalar,D>,std::vector<Scalar>>;
                ret_vec.reserve(V.size());
                int rows = V[0].rows();
                std::map<StlVec,int> vmap;
                StlVec tmp;
                if constexpr(!static_size) {
                    tmp.resize(rows);
                }
                for(size_t i=0; i<V.size(); ++i) {
                    mtao::eigen::stl2eigen(tmp) = V[i];
                    if(auto it = vmap.find(tmp); it == vmap.end()) {
                        remap[i] = vmap.size();
                        vmap[tmp] = vmap.size();
                    } else {
                        remap[i] = it->second;
                    }
                }
                ret_vec.resize(vmap.size());
                for(auto&& [v,i]: vmap) {
                    ret_vec[i] = mtao::eigen::stl2eigen(v);
                }

            } else {
                KDTree<Scalar,D> tree;
                tree.reserve(V.size());
                std::map<size_t,size_t> remap;
                for(size_t i=0; i<V.size(); ++i) {
                    remap[i] = tree.pruning_insertion(V[i],eps);
                }
                ret_vec = tree.points();

            }
            return std::make_tuple(ret_vec,remap);
        }
    template <typename DerivedV, typename DerivedF, typename T
        >
        auto prune_with_map(const Eigen::MatrixBase<DerivedV>& V, const Eigen::MatrixBase<DerivedF>& F, T eps = T(1e-8)) {
            constexpr static int D = DerivedV::RowsAtCompileTime;
            using Scalar = typename DerivedV::Scalar;
            mtao::vector<typename mtao::Vector<Scalar,D>> stlV(V.cols());
            for(int i = 0; i < V.cols(); ++i) {
                stlV[i] = V.col(i);
            }
            auto pr = prune(stlV,eps);
            //auto [P,m] = prune(stlV,eps);
            const mtao::vector<typename mtao::Vector<Scalar,D>>& P = std::get<0>(pr);
            const std::map<int,int>& m = std::get<1>(pr);


            mtao::ColVectors<Scalar,D> RV(V.rows(),P.size());
            for(int i = 0; i < RV.cols(); ++i) {
                RV.col(i) = P[i];
            }
            using IndexType = typename DerivedF::Scalar;
            auto RFp = F.unaryExpr([&](const IndexType& idx) -> IndexType {
                    return m.at(idx);
                    }).eval();

            std::set<int> good_idx;
            for(int i = 0; i < RFp.cols(); ++i) {
                auto v = RFp.col(i);
                std::set<IndexType> indx;
                for(int j = 0; j < RFp.rows(); ++j) {
                    indx.insert(v(j));
                }
                if(indx.size() == RFp.rows()) {
                    good_idx.insert(i);
                }

            }
            decltype(RFp) RF(RFp.rows(),good_idx.size());
            for(auto&& [i,j]: iterator::enumerate(good_idx) ){
                RF.col(i) = RFp.col(j);
            }


            return std::make_tuple(RV,RF,m);

        }
    template <typename DerivedV, typename DerivedF
            ,typename T = typename DerivedV::Scalar>
        auto prune(const Eigen::MatrixBase<DerivedV>& V, const Eigen::MatrixBase<DerivedF>& F, T eps = T(1e-8)) {
            auto [RV,RF,m] = prune_with_map(V,F,eps);
            return std::make_tuple(RV,RF);
        }

}}
