#pragma once
#include "indexer_base.hpp"
#include "mtao/algebra/horner_evaluation.hpp"
#include <iostream>
#include <functional>
#include "mtao/type_utils.h"
namespace mtao {
    namespace geometry {
        namespace grid {
            namespace indexing {
                template <int D_, bool RowMajor = false>
                    class OrderedIndexer: public IndexerBase<D_,OrderedIndexer<D_>>{
                        public:
                            static constexpr int D = D_;
                            using Base = IndexerBase<D_,OrderedIndexer<D_>>;
                            using coord_type = typename Base::coord_type;
                            using Base::Base;
                            using Base::resize;
                            using Base::size;
                            using Base::shape;
                            using Base::rows;
                            using Base::cols;
                            template <typename ArrType>
                                size_t index_from_array(const ArrType& arr) const {
                                    if constexpr(RowMajor) {
                                        return ::mtao::algebra::horner_rowmajor_index(arr,shape());
                                    } else {
                                        return ::mtao::algebra::horner_rowminor_index(arr,shape());
                                    }
                                }
                            coord_type unindex(size_t ind) const {
                                coord_type c;
                                if constexpr(RowMajor) {
                                auto v = ::mtao::algebra::horner_rowmajor_inverse_index(ind,shape());
                                std::transform(v.begin(),v.end(),c.begin(),[](size_t a) -> int { return a; });
                                } else {
                                auto v = ::mtao::algebra::horner_rowminor_inverse_index(ind,shape());
                                std::transform(v.begin(),v.end(),c.begin(),[](size_t a) -> int { return a; });
                                }
                                return c;
                            }

                            /*
                            template <int Dim=0, typename... Args>
                                size_t index(size_t a,Args... args) const {
                                    return index(coord_type{{
                                    assert(int(a) < shape(Dim));
                                    return a + shape(Dim) * index<Dim+1>(args...);
                                }
                            template <int Dim=D-1>
                                size_t index(size_t a) const {
                                    static_assert(Dim == D-1 || Dim == 0,"");
                                    assert(int(a) < shape(Dim));
                                    return a;
                                }
                                */


                    };
            }
        }
    }
}
