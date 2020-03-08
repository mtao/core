#pragma once 
#include "mtao/types.hpp"
#include "mtao/iterator/enumerate.hpp"
#include <numeric>

namespace mtao::geometry::mesh {

    // either pass in itersatosr full of tuples {Vertices, Triangles} or tuples {Vertices,Triangles, colors}
    template <typename BeginIt, typename EndIt>
        auto stack_meshes(BeginIt beginit, EndIt endit) {
            using TupleType = std::decay_t<decltype(*beginit)>;
            using VType = std::tuple_element_t<0,TupleType>;
            using T = typename VType::Scalar;
            constexpr int D = VType::RowsAtCompileTime;
            constexpr bool has_color = std::tuple_size_v<TupleType> == 3;

            std::vector<int> sizes(std::distance(beginit,endit));
            std::transform(beginit,endit,sizes.begin(),[](auto&& pr) {
                    return std::get<0>(pr).cols();
                    });

            std::vector<int> offsets(sizes.size()+1);
            offsets[0] = 0;
            std::partial_sum(sizes.begin(),sizes.end(),offsets.begin()+1);

            std::vector<int> fsizes(sizes.size());
            std::transform(beginit,endit,fsizes.begin(),[](auto&& pr) {
                    return std::get<1>(pr).cols();
                    });
            std::vector<int> foffsets(fsizes.size()+1);
            foffsets[0] = 0;
            std::partial_sum(fsizes.begin(),fsizes.end(),foffsets.begin()+1);

            mtao::ColVectors<T,D> V(std::get<0>(*beginit).rows(),offsets.back());

            mtao::ColVectors<T,4> C;

            if constexpr(has_color) {
                C.resize(4,V.cols());
            }
            mtao::ColVecs3i F(3,foffsets.back());
            for(auto&& [idx,pr,off,foff]: mtao::iterator::enumerate(mtao::iterator::shell(beginit,endit),offsets,foffsets)) {
                if constexpr(has_color) {
                    auto&& [v,f,c] = pr;
                    V.block(0,off,v.rows(),v.cols()) = v;
                    F.block(0,foff,f.rows(),f.cols()) = f.array() + off;
                    if(c.cols() == 1) {
                        C.block(0,off,4,v.cols()).colwise() = c;
                    } else {
                        C.block(0,off,4,v.cols()) = c;
                    }
                }else {
                    auto&& [v,f] = pr;
                    V.block(0,off,v.rows(),v.cols()) = v;
                    F.block(0,foff,f.rows(),f.cols()) = f.array() + off;
                }
            }
            if constexpr(has_color) {
                return std::make_tuple(V,F,C);
            } else {
                return std::make_tuple(V,F);
            }

        }
    template <typename Container>
        auto stack_meshes(const Container& container) {
            return stack_meshes(container.begin(),container.end());
        }


}
