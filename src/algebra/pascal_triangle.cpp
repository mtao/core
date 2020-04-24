#include "mtao/algebra/pascal_triangle.hpp"

#include <cassert>
#include <iostream>

namespace mtao::algebra {
size_t PascalTriangle::operator()(size_t level, size_t index) const {
    size_t off = (level * (level + 1)) / 2;
    assert(index < level + 1);
    return _entries.at(off + index);
}
PascalTriangle::PascalTriangle(size_t levels) { set_levels(levels); }
void PascalTriangle::set_levels(size_t size) {
    _levels = size;
    rebuild();
}
void PascalTriangle::rebuild() {
    _entries.resize(((_levels + 1) * (_levels + 2)) / 2);

    _entries[0] = 1;
    for (size_t level = 1; level <= _levels; ++level) {
        // level = 2 then
        // 1 2 1
        // index: 0 1 2 3 4 5
        // value: 1 1 1 1 2 1
        // offset = 2 * (2 + 1) / 2 = 3
        // previous offset = 1 * ( 1 + 1 ) / 2 = 1
        // go from 1 to... 1 = level - 1:)
        size_t off = (level * (level + 1)) / 2;
        size_t last_off = (level * (level - 1)) / 2;
        _entries[off] = 1;
        _entries[off + level] = 1;
        for (size_t j = 1; j < level; ++j) {
            _entries[off + j] =
                _entries[last_off + j - 1] + _entries[last_off + j];
        }
    }
}
}  // namespace mtao::algebra
