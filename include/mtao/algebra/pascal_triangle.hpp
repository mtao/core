#pragma once
#include <vector>

namespace mtao::algebra {

// zero indexed pascal triangle, to match that the first row is 0th order
// polynomials
struct PascalTriangle {
   public:
    PascalTriangle(size_t levels);
    size_t operator()(size_t level, size_t index) const;
    void set_levels(size_t size);
    size_t levels() const { return _levels; }
    const std::vector<size_t>& entries() const { return _entries; }

   private:
    void rebuild();
    std::vector<size_t> _entries;
    size_t _levels;
};
}  // namespace mtao::algebra
