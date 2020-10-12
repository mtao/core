#include <fmt/format.h>

#include <catch2/catch.hpp>
#include <compare>
#include <iostream>
#include <mtao/algebra/partial_order_to_dag.hpp>
//#include "mtao/data_structures//directed_acyclic_graph.hpp"

using namespace mtao::data_structures;
using namespace mtao::algebra;

template <typename T, typename U>
void divisibility_check_recurse(T& value, const U& co, std::string str = "") {
    for (auto&& c : value.children) {
        std::string v = fmt::format(str + "=>{}", co[c->value]);
        std::cout << v << std::endl;
        CHECK(co[c->value] % co[value.value] == 0);

        divisibility_check_recurse(*c, co, v);
    }
}
template <typename T, typename U>
void divisibility_check_recurse_inv(T& value, const U& co,
                                    std::string str = "") {
    for (auto&& c : value.children) {
        std::string v = fmt::format(str + "=>{}", co[c->value]);
        std::cout << v << std::endl;
        CHECK(co[value.value] % co[c->value] == 0);

        divisibility_check_recurse_inv(*c, co, v);
    }
}

TEST_CASE("divisibility_dag", "[po_to_dag]") {
    // std::vector values{1, 2, 3, 6};
    std::vector values{1, 2, 4, 6, 7, 8, 3};
    values.resize(100);
    std::iota(values.begin(), values.end(), 2);
    auto compare = [](int a, int b) -> std::partial_ordering {
        bool lower = a % b == 0;
        bool higher = b % a == 0;
        // spdlog::info("{} <=> {} ({} {})", a, b, lower, higher);
        if (lower && higher) {
            return std::partial_ordering::equivalent;
        } else if (!(lower || higher)) {
            return std::partial_ordering::unordered;
        } else if (lower) {
            return std::partial_ordering::less;
        } else {
            return std::partial_ordering::greater;
        }
        return std::partial_ordering::unordered;
    };

    compare(3, 4);
    auto dag = partial_order_to_dag(values, compare);
    for (auto&& r : dag.roots) {
        std::cout << fmt::format("{}", values[r->value]) << std::endl;
        divisibility_check_recurse(*r, values,
                                   fmt::format("{}", values[r->value]));
    }
}
TEST_CASE("divisibility_dag_inv", "[po_to_dag]") {
    // std::vector values{1, 2, 3, 6};
    std::vector values{1, 2, 4, 6, 7, 8, 3};
    values.resize(100);
    std::iota(values.begin(), values.end(), 2);
    auto compare = [](int a, int b) -> std::partial_ordering {
        bool higher = a % b == 0;
        bool lower = b % a == 0;
        // spdlog::info("{} <=> {} ({} {})", a, b, lower, higher);
        if (lower && higher) {
            return std::partial_ordering::equivalent;
        } else if (!(lower || higher)) {
            return std::partial_ordering::unordered;
        } else if (lower) {
            return std::partial_ordering::less;
        } else {
            return std::partial_ordering::greater;
        }
        return std::partial_ordering::unordered;
    };

    compare(3, 4);
    auto dag = partial_order_to_dag(values, compare);
    for (auto&& r : dag.roots) {
        std::cout << fmt::format("inv{}", values[r->value]) << std::endl;
        divisibility_check_recurse_inv(*r, values,
                                       fmt::format("inv{}", values[r->value]));
    }
}
