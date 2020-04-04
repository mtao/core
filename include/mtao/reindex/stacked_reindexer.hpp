#pragma once
#include <map>
#include <mtao/iterator/zip.hpp>
#include <vector>

#include "mtao/reindex/offset_reindexer.hpp"

namespace mtao::reindex {

// A reindexer that takes in a collection of intervals and pulls them together
// This reindexer only accepts integer types, useful for shifting indices up and
// down
template <typename IndexType = size_t>
struct StackedReindexer
    : public ReindexerBase<OffsetReindexer<std::pair<size_t, IndexType>>,
                           std::pair<size_t, IndexType>> {
   public:
    StackedReindexer(const std::vector<size_t>& sizes);
    using OffsetReindexerType = OffsetReindexer<IndexType>;
    using IndexPair = std::pair<size_t, IndexType>;
    size_t index(size_t a, const IndexType& ind) const {
        return index(IndexPair(a, ind));
    }
    size_t index(const IndexPair& idx) const {
        return _blocks.at(std::get<0>(idx)).index(std::get<1>(idx));
    }
    IndexPair unindex(size_t idx) const;
    size_t contains_original(const IndexPair& idx) const;
    size_t size() const { return _blocks.back().end(); }

    size_t find_block(const IndexType& index) const;

   private:
    std::vector<OffsetReindexerType> _blocks;
};
template <typename IndexType>
StackedReindexer<IndexType>::StackedReindexer(
    const std::vector<size_t>& sizes) {
    _blocks.resize(sizes.size());
    size_t offset = 0;
    for (auto&& [block, size] : iterator::zip(_blocks, sizes)) {
        block.set_size(size);
        block.set_offset(offset);
        offset += size;
    }
}
template <typename IndexType>
size_t StackedReindexer<IndexType>::find_block(const IndexType& index) const {
    // binary search for where this may belong
    assert(_blocks.size() > 1);
    size_t min = 0;
    size_t max = _blocks.size() - 1;

    while (min != max) {
        size_t mid = (min + max) / 2;
        auto& block = _blocks.at(mid);
        if (block.offset() > index) {
            max = mid - 1;
        } else if (size_t block_end = block.end();
                   static_cast<size_t>(index) >= block_end) {
            min = mid + 1;
        } else {
            return mid;
        }
    }
    return min;
}
template <typename IndexType>
size_t StackedReindexer<IndexType>::contains_original(
    const IndexPair& idx) const {
    return _blocks.at(std::get<0>(idx)).contains(std::get<1>(idx));
}
template <typename IndexType>
auto StackedReindexer<IndexType>::unindex(size_t idx) const -> IndexPair {
    size_t blockidx = find_block(idx);
    const auto& block = _blocks.at(blockidx);
    return std::make_pair(blockidx, block.unindex(idx));
}
}  // namespace mtao::reindex

