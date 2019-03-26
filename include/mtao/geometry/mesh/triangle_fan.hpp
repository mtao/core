#pragma once
#include "mtao/types.hpp"
#include "mtao/type_utils.h"
#include "mtao/eigen/stl2eigen.hpp"
#include "mtao/iterator/shell.hpp"
#include <list>


namespace mtao::geometry::mesh {
    template <typename BeginIt, typename EndIt>
    std::vector<std::array<int,3>> triangle_fan_stl(const BeginIt& beginit, const EndIt& endit) {
        using Face = std::array<int,3>;
        std::vector<Face> stlF;
        if constexpr(std::is_integral_v<mtao::types::remove_cvref_t<decltype(*beginit)>>) {
            stlF.reserve(std::distance(beginit,endit) - 2);
            Face f;
            auto it = beginit;
            f[0] = *(it++);
            f[2] = *(it++);
            for(; it != endit; ++it) {
                f[1] = f[2];
                f[2] = *it;
                stlF.push_back(f);
            }
        } else {
            size_t size = 0;
            for(auto&& c: mtao::iterator::shell(beginit,endit)) {
                size += c.size() - 2;
            }
            stlF.reserve(size);

            for(auto&& c: mtao::iterator::shell(beginit,endit)) {
                auto F = triangle_fan_stl(c.begin(),c.end());
                stlF.insert(stlF.end(),F.begin(),F.end());
            }
        }
        return stlF;
    }
    template <typename Container>
        mtao::ColVectors<int,3> triangle_fan(const Container& C) {
            return eigen::stl2eigen(triangle_fan_stl(C.begin(),C.end()));
        }
    template <typename T>
        mtao::ColVectors<int,3> triangle_fan(const std::initializer_list<T>& C) {
            return eigen::stl2eigen(triangle_fan_stl(C.begin(),C.end()));
        }
}
