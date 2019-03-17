#pragma once
#include "mtao/types.hpp"
#include "mtao/type_utils.h"
#include "mtao/iterator/enumerate.hpp"
#include "mtao/geometry/winding_number.hpp"
#include "mtao/eigen/stl2eigen.hpp"
#include "mtao/iterator/shell.hpp"
#include <list>


namespace mtao::geometry::mesh {
    template <typename VDerived,typename BeginIt, typename EndIt>
    std::vector<std::array<int,3>> earclipping_stl(const Eigen::MatrixBase<VDerived>& V, const BeginIt& beginit, const EndIt& endit) {
        using Face = std::array<int,3>;
        std::vector<Face> stlF;
        if constexpr(std::is_integral_v<mtao::types::remove_cvref_t<decltype(*beginit)>>) {
            stlF.reserve(std::distance(beginit,endit) - 2);
            using Scalar = typename VDerived::Scalar;
            std::list<int> CL(beginit,endit);
            while(CL.size() > 3) {
                for(auto it = CL.begin(); it != CL.end(); ++it) {
                    auto it1 = it;
                    it1++;
                    if(it1 == CL.end()) { it1 = CL.begin(); }
                    auto it2 = it1;
                    it2++;
                    if(it2 == CL.end()) { it2 = CL.begin(); }
                    const Face f{{*it,*it1,*it2}};
                    bool is_earclip = true;
                    for(auto mit = beginit; mit != endit; ++mit) {
                        int i = *mit;
                        if( i == f[0] || i == f[1] || i == f[2] ) {
                            continue;
                        }
                        auto v = V.col(i);
                        if(interior_winding_number(V,f,v)) {
                            is_earclip = false;
                            break;
                        }
                    }
                    if(is_earclip) {
                        stlF.push_back(f);
                        CL.erase(it1);
                        break;
                    }

                }
            }

            Face f;
            std::copy(CL.begin(),CL.end(),f.begin());
            stlF.push_back(f);
        } else {
            size_t size = 0;
            for(auto&& c: mtao::iterator::shell(beginit,endit)) {
                size += c.size() - 2;
            }
            stlF.reserve(size);

            for(auto&& c: mtao::iterator::shell(beginit,endit)) {
                auto F = earclipping_stl(V,c.begin(),c.end());
                stlF.insert(stlF.end(),F.begin(),F.end());
            }
        }
        return stlF;
    }
    template <typename VDerived, typename Container>
    mtao::ColVectors<int,3> earclipping(const Eigen::MatrixBase<VDerived>& V, const Container& C) {
        return eigen::stl2eigen(earclipping_stl(V,C.begin(),C.end()));
    }
    template <typename VDerived, typename T>
    mtao::ColVectors<int,3> earclipping(const Eigen::MatrixBase<VDerived>& V, const std::initializer_list<T>& C) {
        return eigen::stl2eigen(earclipping_stl(V,C.begin(),C.end()));
    }
}