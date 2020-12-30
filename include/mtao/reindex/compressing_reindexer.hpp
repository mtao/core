#pragma once
#include <map>
#include <vector>

#include "mtao/reindex/reindexer_base.hpp"

namespace mtao::reindex {

// Compresses a set of integers to sequential set of integers; useful
template<typename IndexType = size_t>
struct CompressingReindexer
  : public ReindexerBase<CompressingReindexer<IndexType>, IndexType> {
  public:
    template<typename... Args>
    size_t add(Args &&... args) {
        IndexType val(std::forward<Args>(args)...);
        auto [it, happened] = _index_map.try_emplace(val, _unindex_vec.size());
        if (happened) {
            _unindex_vec.emplace_back(val);
        }
        return it->second;
    }

    // a relatively efficient way to add a bunch of new indices
    template<typename Container>
    void add_multiple(Container &&items) {
        reserve(size() + items.size());
        for (auto &&item : items) {
            add(item);
        }
    }

    // a relatively efficient way to add a bunch of new indices
    template<typename T>
    void add_multiple(std::initializer_list<T> items) {
        reserve(size() + items.size());
        for (auto &&item : items) {
            add(item);
        }
    }

    // something to improve adding performance
    void reserve(size_t size) { _unindex_vec.reserve(size); }

    template<typename... Args>
    size_t index(Args &&... args) {
        return _index_map.at(IndexType(std::forward<Args>(args)...));
    }
    IndexType unindex(size_t idx) const { return _unindex_vec.at(idx); }
    size_t contains_original(const IndexType &idx) const {
        return _index_map.find(idx) != _index_map.end();
    }
    size_t size() const { return _unindex_vec.size(); }
    const std::map<IndexType, size_t> &index_map() const { return _index_map; }
    const std::vector<IndexType> &unindex_vec() const { return _unindex_vec; }

  private:
    std::map<IndexType, size_t> _index_map;
    std::vector<IndexType> _unindex_vec;
};
}// namespace mtao::reindex
