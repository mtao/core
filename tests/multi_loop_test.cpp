#include <iostream>
#include <iterator>
#include <array>
#include <mutex>
#include <mtao/geometry/grid/grid_utils.h>
#include <set>
#include <fmt/printf.h>
#include <catch2/catch.hpp>


TEST_CASE("multi_loops", "[multiloop]") {
    std::array<int, 3> begin, end;
    std::fill(begin.begin(), begin.end(), 5);
    end[0] = 10;
    end[1] = 11;
    end[2] = 12;

    //mtao::multi_loop(end,[&](auto&& idx) {
    //        std::copy(idx.begin(),idx.end(),std::ostream_iterator<int>(std::cout," "));
    //        std::cout << std::endl;
    //        });
    {
        std::set<std::array<int, 3>> seen;
        mtao::geometry::grid::utils::multi_loop(begin, end, [&](auto &&idx) {
            seen.insert(idx);
            for (int j = 0; j < 3; ++j) {
                REQUIRE(begin[j] <= idx[j]);
                REQUIRE(idx[j] < end[j]);
            }
        });
        REQUIRE(seen.size() == 5 * 6 * 7);
    }

    {
        std::set<std::array<int, 3>> seen;
        std::mutex seen_mut;
        mtao::geometry::grid::utils::multi_loop_parallel(begin, end, [&](auto &&idx) {
            {
                std::scoped_lock lock(seen_mut);
                seen.insert(idx);
            }
            for (int j = 0; j < 3; ++j) {
                REQUIRE(begin[j] <= idx[j]);
                REQUIRE(idx[j] < end[j]);
            }
            //fmt::print("{} {} {}\n", idx[0], idx[1], idx[2]);
        });
        REQUIRE(seen.size() == 5 * 6 * 7);
    }
    std::set<std::array<int, 2>> seen;
    mtao::geometry::grid::utils::multi_loop(std::array<int, 2>{ { 2, 3 } }, [&](auto &&idx) {
        seen.insert(idx);
        for (int j = 0; j < 2; ++j) {
            REQUIRE(0 <= idx[j]);
            REQUIRE(idx[j] < j + 2);
        }
    });
    REQUIRE(seen.size() == 2 * 3);
    seen.clear();
    mtao::geometry::grid::utils::masked_multi_loop<1>(std::array<int, 2>{ { 2, 3 } }, [&](auto &&idx) {
        seen.insert(idx);
        REQUIRE(idx[1] == 0);

        REQUIRE(0 <= idx[0]);
        REQUIRE(idx[0] < 2);
    });
    REQUIRE(seen.size() == 2);
    seen.clear();
    mtao::geometry::grid::utils::masked_multi_loop<2>(std::array<int, 2>{ { 2, 3 } }, [&](auto &&idx) {
        seen.insert(idx);
        REQUIRE(idx[0] == 0);

        REQUIRE(0 <= idx[1]);
        REQUIRE(idx[1] < 3);
    });
    REQUIRE(seen.size() == 3);
    seen.clear();
    mtao::geometry::grid::utils::masked_multi_loop<3>(std::array<int, 2>{ { 2, 3 } }, [&](auto &&idx) {
        seen.insert(idx);
        REQUIRE(0 <= idx[0]);
        REQUIRE(idx[0] < 2);

        REQUIRE(0 <= idx[1]);
        REQUIRE(idx[1] < 3);
    });
    REQUIRE(seen.size() == 2 * 3);
    seen.clear();
}
