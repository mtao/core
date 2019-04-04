#pragma once
#include "mtao/types.hpp"
#include <iostream>
#include "mtao/type_utils.h"
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
            size_t size = std::distance(beginit,endit);
            stlF.reserve(size - 2);
            double ang_sum = 0;
            for(auto it = beginit; it != endit; ++it) {
                auto it1 = it;
                it1++;
                if(it1 == endit) { it1 = beginit; }
                auto it2 = it1;
                it2++;
                if(it2 == endit) { it2 = beginit; }
                auto a = V.col(*it);
                auto b = V.col(*it1);
                auto c = V.col(*it2);
                double ang = mtao::geometry::trigonometry::angle(c-b,a-b)(0);
                ang_sum += ang;
            }

            double expected_total_ang = mtao::geometry::trigonometry::interior_angle_sum(size);
            //double unexpected_total_ang = mtao::geometry::trigonometry::exterior_angle_sum(size);
            bool reverse_orientation = std::abs(expected_total_ang - ang_sum) > 1e-2;

            std::list<int> CL(beginit,endit);

            auto is_earclip = [&](const Face &f) -> bool {

                auto a = V.col(f[0]);
                auto b = V.col(f[1]);
                auto c = V.col(f[2]);
                auto cb = c-b;
                auto ab = a-b;
                if(cb.y() * ab.x() -  cb.x() * ab.y() < 1e-10 ) {
                    return false;
                }
                /*
                if(cb.y() * ab.x() -  cb.x() * ab.y() < 1e-10 ) {
                    return false;
                }
                */
                double ang = mtao::geometry::trigonometry::angle(cb,ab)(0);
                if(ang >= M_PI) {
                    return false;
                }

                for(auto mit = beginit; mit != endit; ++mit) {
                    int i = *mit;
                    if( i == f[0] || i == f[1] || i == f[2] ) {
                        continue;
                    }
                    auto v = V.col(i);
                    if(interior_winding_number(V,f,v)) {
                        return false;
                    }
                }
                return true;
            };

            while(CL.size() > 3) {
                bool earclipped = false;
                for(auto it = CL.begin(); it != CL.end(); ++it) {
                    auto it1 = it;
                    it1++;
                    if(it1 == CL.end()) { it1 = CL.begin(); }
                    auto it2 = it1;
                    it2++;
                    if(it2 == CL.end()) { it2 = CL.begin(); }
                    Face f{{*it,*it1,*it2}};
                    if(reverse_orientation) {
                        std::swap(f[0],f[2]);
                    }
                    if(is_earclip(f)) {
                        stlF.push_back(f);
                        CL.erase(it1);
                        earclipped = true;
                        break;
                    }

                }
                if(!earclipped) {
                    std::cerr << "Earclipping failed!" << std::endl;
                    auto it = CL.begin();
                    auto it1 = it;
                    it1++;
                    auto it2 = it1;
                    it2++;
                    const Face f{{*it,*it1,*it2}};
                    stlF.push_back(f);
                    CL.erase(it1);

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
                if(c.size() >= 3) {
                    auto F = earclipping_stl(V,c.begin(),c.end());
                    stlF.insert(stlF.end(),F.begin(),F.end());
                }
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
