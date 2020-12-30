#pragma once

namespace mtao::reindex {

// Compresses a set of integers to sequential set of integers; useful
template<typename Derived, typename IndexType>
struct ReindexerBase {
  public:
    Derived &derived() { return *static_cast<Derived *>(this); }
    const Derived &derived() const {
        return *static_cast<const Derived *>(this);
    }
    // size_t add(const IndexType& idx) { derived().add(idx); }
    size_t index(const IndexType &idx) const { return derived().index(idx); }
    IndexType unindex(size_t idx) const { return derived().unindex(idx); }
    size_t size() const { return derived().size(); }
    // whether this reindexer contains the following internal index
    bool contains_original(const IndexType &idx) const {
        return derived().contains_original(idx);
    }
    // whether this reindexer contains the following in its indexed index
    // range
    bool contains_indexed(size_t idx) const {
        return idx < size();
    }// size_t => unsigned so just have to check
};
}// namespace mtao::reindex
