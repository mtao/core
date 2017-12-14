#pragma once
#include "types.h"

namespace mtao {

    namespace detail {
        template <typename ColVecType>
            struct colvector_loop {
                using colvec_type = ColVecType;
                public:
                    struct iterator {
                        public:
                            iterator(colvec_type* V, int idx): V(V), idx(idx) {}
                            auto operator*() { return V->col(idx); }

                            bool operator<(iterator& other) const {
                                return this->idx < other.idx;
                            }
                            bool operator!=(iterator& other) const {
                                return this->idx != other.idx;
                            }
                            iterator& operator++() {idx++; return *this; }
                        private:
                            colvec_type* V;
                            int idx;
                    };
                    colvector_loop(colvec_type& V): V(&V) {}
                    auto begin() { return iterator(V,0); }
                    auto end() { return iterator(V,V->cols()); }

                private:
                    colvec_type* V;
            };
    }

    template <typename T, int D>
        auto colvector_loop(const mtao::ColVectors<T,D>& V) {
            return detail::colvector_loop<const mtao::ColVectors<T,D>>(V);
        }
    template <typename T, int D>
        auto colvector_loop(mtao::ColVectors<T,D>& V) {
            return detail::colvector_loop<mtao::ColVectors<T,D>>(V);
        }

}
