#pragma once
#include <map>
#include <vector>

#include "mtao/reindex/reindexer_base.hpp"

namespace mtao::reindex {

// only accepts integer types, useful for shifting indices up and down
template<typename IndexType = size_t>
struct OffsetReindexer
  : public ReindexerBase<OffsetReindexer<IndexType>, IndexType> {
  public:
    void set_size(size_t size) { _size = size; }
    void set_offset(long long offset) { _offset = offset; }
    size_t index(const IndexType &idx) const { return idx + _offset; }
    size_t unindex(size_t idx) const { return idx - _offset; }
    size_t contains_original(const IndexType &idx) const {
        return idx < size();
    }
    size_t size() const { return _size; }
    size_t end() const {
        return size() + offset();
    }// the index afte rthe last valid one

    long long offset() const { return _offset; }

  private:
    size_t _size = 0;
    long long _offset = 0;// could be positive or negative!
};
}// namespace mtao::reindex
