#pragma once
#include <map>
#include <vector>
#include <array>
#include "mtao/iterator/zip.hpp"


namespace mtao {
    template <typename IndexType>
        struct ReIndexer {
            IndexType add_index(IndexType idx) {
                if(auto it = m_forward.find(idx); it != m_forward.end()) {
                    return it->second;
                } else {
                    IndexType new_idx = m_forward.size();
                    return m_forward[idx] = new_idx;
                }
            }

            IndexType operator()(IndexType idx) {
                return add_index(idx);
            }
            IndexType get(IndexType idx) const {
                return m_forward[idx]; 
            }
            bool has_index(IndexType idx) const {
                return m_forward.find(idx) != m_forward.end();
            }
            std::vector<IndexType> inverse() const {
                std::vector<IndexType> ret(m_forward.size());
                for(auto&& [k,v]: m_forward) {
                    ret[v] = k;
                }
                return ret;
            }
            auto&& map() const { return m_forward; }
            size_t size() const { return m_forward.size(); }
            private:
            std::map<IndexType,IndexType> m_forward;
        };
    template <typename IndexType, size_t Size>
        struct StackedReIndexer {
            using RET = ReIndexer<IndexType>;
            template <size_t D>
            void add_index(IndexType idx) {
                indexers[D].add_index(idx);
                for(size_t i = D; i < Size; ++i) {
                    offsets[i+1] = offsets[i] + indexers[i].size();
                }
            }
            size_t size() const {
                return offsets[Size];
            }
            template <int D>
            bool has_index(IndexType idx) const {
                return indexers[D].has_index(idx);
            }
            std::vector<IndexType> inverse() const {
                std::vector<IndexType> ret(size());
                for(auto&& [i,o]: mtao::iterator::zip(indexers,offsets)) {
                    for(auto&& [k,v]: i.map()) {
                        ret[v + o] = k;
                    }
                }
                return ret;
            }
            std::array<RET,Size> indexers;
            std::array<size_t,Size+1> offsets = {};

        };
}
