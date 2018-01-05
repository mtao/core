#pragma once
#include <mtao/types.h>


namespace mtao { namespace geometry { namespace mesh {

    template <int D>
        struct CellIndexer {
            public:

                template <typename Derived>
                std::array<int,D> sorted_facet(const Eigen::DenseBase<Derived>& f) const {
                        std::array<int,D> ret;
                        for(int i = 0; i < D; ++i) {
                            ret[i] = f(i);
                        }
                        std::sort(ret.begin(),ret.end());
                        return ret;
                }

                CellIndexer(const mtao::ColVectors<int,D>& F) {
                    for(int i = 0; i < F.cols(); ++i) {
                        m_map[sorted_facet(F.col(i))] = i;
                    }
                }
                template <typename Derived>
                int operator()(const Eigen::DenseBase<Derived>& f) const {
                    return (*this)(sorted_fact(f));
                }
                int operator()(std::array<int,D> f) const {
                    std::sort(f.begin(),f.end());
                    return m_map.at(f);
                }

            private:
                std::map<std::array<int,D>,int> m_map;

        };
}}}
