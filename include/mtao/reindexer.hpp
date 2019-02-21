#pragma once
#include <map>
#include <vector>


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
    private:
    std::map<IndexType,IndexType> m_forward;
};
}
