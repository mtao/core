#include <iostream>
#include <mtao/tbb/parallel_reduce.hpp>
#include <catch2/catch.hpp>
#include <iostream>
#include <fmt/format.h>

TEST_CASE("Parallel reduce", "[tbb]") {

    auto ret = tbb::parallel_reduce(tbb::blocked_range<size_t>(0,200), std::vector<int>{},
            [](const tbb::blocked_range<size_t>& range, std::vector<int> ret) -> std::vector<int> {
            //ret.reserve(ret+range.size());
            for(size_t i = range.begin(); i != range.end(); ++i) {
            ret.push_back(i);
            }
            return ret;
            },
            [](std::vector<int> a, const std::vector<int>& b) -> std::vector<int> {
            a.insert(a.end(),b.begin(),b.end());
            return a;
            }
            );
    std::sort(ret.begin(), ret.end());
    for(size_t i = 0; i < ret.size(); ++i) {
        REQUIRE( ret[i] == i );
    }
}
