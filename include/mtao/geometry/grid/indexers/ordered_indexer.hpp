#pragma once
#include "indexer_base.hpp"
#include "mtao/algebra/horner_evaluation.hpp"
#include <iostream>
namespace mtao {
    namespace geometry {
        namespace grid {
            namespace indexing {
                template <int D_, bool RowMajor = false>
                    class OrderedIndexer: public IndexerBase<D_,OrderedIndexer<D_>>{
                        public:
                            static constexpr int D = D_;
                            using Base = IndexerBase<D_,OrderedIndexer<D_>>;
                            using index_type = typename Base::index_type;
                            using Base::Base;
                            using Base::resize;
                            using Base::size;
                            using Base::shape;
                            using Base::rows;
                            using Base::cols;
                            template <typename... Args>
                                size_t index(Args... args) const {
                                    return index(index_type{{args...}});
                                }
                            size_t index(const index_type& a) const {
                                if constexpr(RowMajor) {
                                return ::mtao::algebra::horner_rowmajor_index(a,shape());
                                } else {
                                return ::mtao::algebra::horner_rowminor_index(a,shape());
                                }
                            }

                            /*
                            template <int Dim=0, typename... Args>
                                size_t index(size_t a,Args... args) const {
                                    return index(index_type{{
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
