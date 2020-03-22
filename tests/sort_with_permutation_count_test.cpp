#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/algebra/sort_with_permutation_sign.hpp>

using namespace mtao::algebra;

namespace {
template <typename T>
void sort_test(T vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        vec[i] = vec.size() - i - 1;
    }
    bool sign = (vec.size() / 2) % 2 == 0;
    {
        auto [svec, size] = sort_with_permutation_sign<SortType::Bubble>(vec);
        REQUIRE(std::is_sorted(svec.begin(), svec.end()));
        REQUIRE(sign == size);
    }
    {
        auto [svec, size] =
            sort_with_permutation_sign<SortType::Insertion>(vec);
        REQUIRE(std::is_sorted(svec.begin(), svec.end()));
        REQUIRE(sign == size);
    }
    {
        auto [svec, size] =
            sort_with_permutation_sign<SortType::Selection>(vec);
        REQUIRE(std::is_sorted(svec.begin(), svec.end()));
        REQUIRE(sign == size);
    }
}
}  // namespace

TEST_CASE("SortWithSigns", "[sorting]") {
    for (int j = 0; j < 10; ++j) {
        std::vector<int> vec(j);
        sort_test(vec);
    }
    std::cout << std::endl;
    sort_test(std::array<int, 1>{{}});
    sort_test(std::array<int, 2>{{}});
    sort_test(std::array<int, 3>{{}});
    sort_test(std::array<int, 4>{{}});
    sort_test(std::array<int, 5>{{}});
    sort_test(std::array<int, 6>{{}});
    sort_test(std::array<int, 7>{{}});
    sort_test(std::array<int, 8>{{}});
    sort_test(std::array<int, 9>{{}});
}
