#include <iostream>
#include <mtao/tbb/parallel_reduce.hpp>
#include <map>
#include <catch2/catch.hpp>
#include <iostream>
#include <fmt/format.h>

TEST_CASE("Parallel reduce", "[tbb]") {

    std::map<int, int> mp;
    mp[0] = 1;
    mp[1] = 1;
    mp[2] = 1;
    mp[3] = 1;
    mp[4] = 1;
    mp[5] = 1;
    {
        auto ret = tbb::parallel_reduce(
          tbb::blocked_range<size_t>(0, 200), std::vector<int>{}, [](const tbb::blocked_range<size_t> &range, std::vector<int> ret) -> std::vector<int> {
                //ret.reserve(ret+range.size());
                for(size_t i = range.begin(); i != range.end(); ++i) {
                ret.push_back(i);
                }
                return ret; }, [](std::vector<int> a, const std::vector<int> &b) -> std::vector<int> {
                a.insert(a.end(),b.begin(),b.end());
                return a; });
        std::sort(ret.begin(), ret.end());
        for (size_t i = 0; i < ret.size(); ++i) {
            REQUIRE(ret[i] == i);
        }
    }

    /*
    {
        auto ret = tbb::parallel_reduce(tbb::blocked_range<std::map<int,int>::const_iterator>(mp.begin(),mp.end()), std::vector<int>{},
                [](const tbb::blocked_range<std::map<int,int>::const_iterator>& range, std::vector<int> ret) -> std::vector<int> {
                //ret.reserve(ret+range.size());
                for(auto it = range.begin(); it != range.end(); ++it) {
                ret.push_back(it.first);
                }
                return ret;
                },
                [](std::vector<int> a, const std::vector<int>& b) -> std::vector<int> {
                a.insert(a.end(),b.begin(),b.end());
                return a;
                }
                );
    }
    */
}
