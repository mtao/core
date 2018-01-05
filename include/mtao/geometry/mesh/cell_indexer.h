#pragma once
#include <mtao/types.h>


namespace mtao { namespace geometry { namespace mesh {

    template <int D, typename IndexType = int>
        struct CellIndexer {
            public:

                template <typename Derived>
                std::array<IndexType,D> sorted_facet(const Eigen::DenseBase<Derived>& f) const {
                        std::array<IndexType,D> ret;
                        for(IndexType i = 0; i < D; ++i) {
                            ret[i] = f(i);
                        }
                        std::sort(ret.begin(),ret.end());
                        return ret;
                }

                CellIndexer(const mtao::ColVectors<IndexType,D>& F) {
                    for(IndexType i = 0; i < F.cols(); ++i) {
                        m_map[sorted_facet(F.col(i))] = i;
                    }
                }
                template <typename Derived>
                IndexType operator()(const Eigen::DenseBase<Derived>& f) const {
                    return (*this)(sorted_fact(f));
                }
                IndexType operator()(std::array<IndexType,D> f) const {
                    std::sort(f.begin(),f.end());
                    return m_map.at(f);
                }

            private:
                std::map<std::array<IndexType,D>,IndexType> m_map;

        };
}}}
