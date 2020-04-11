#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/algebra/composite.hpp>

using namespace mtao::algebra;

TEST_CASE("Composite", "[composite]") {
    std::set<std::tuple<char, int>> ctoi;
    ctoi.emplace('r', 0);
    ctoi.emplace('1', 2);
    ctoi.emplace('2', 4);

    std::set<std::tuple<int, std::string>> itos;
    itos.emplace(0, "zero");
    itos.emplace(2, "two");
    itos.emplace(3, "three");

    auto comp = composite(ctoi, itos);

    REQUIRE(comp.size() == 2);
    auto it = comp.begin();
    {
        auto [a, b] = *it++;
        REQUIRE(a == '1');
        REQUIRE(b == "two");
    }
    {
        auto [a, b] = *it++;
        REQUIRE(a == 'r');
        REQUIRE(b == "zero");
    }
}
/*
TEST_CASE("graph_adjacency_contraction", "[composite]") {
    std::set<std::tuple<int, int>> s;
    s.emplace(0, 1);
    s.emplace(1, 2);

    bool done;
    std::tie(s, done) = composite_with_leftovers(s, s);
    {
        REQUIRE(done == true);
        REQUIRE(s.size() == 1);
        auto [a, b] = *s.begin();
        REQUIRE(a == 0);
        REQUIRE(b == 2);
    }
}
TEST_CASE("graph_adjacency_contraction2", "[composite]") {
    std::set<std::tuple<int, int>> s;
    s.emplace(0, 1);
    s.emplace(1, 2);
    s.emplace(2, 3);

    bool done;
    std::tie(s, done) = composite_with_leftovers(s, s);
    for(auto&& [a,b]: s) {
        std::cout << a << " => " << b << std::endl;
    }
    {
        REQUIRE(done == true);
        REQUIRE(s.size() == 2);
        {
            auto [a, b] = *it++;
            REQUIRE(a == 0);
            REQUIRE(b == 2);
        }
        {
            auto [a, b] = *it++;
            REQUIRE(a == 0);
            REQUIRE(b == 3);
        }
    }
    std::tie(s, done) = composite_with_leftovers(s, s);
    {
        REQUIRE(done == true);
        REQUIRE(s.size() == 1);
        auto [a, b] = *s.begin();
        REQUIRE(a == 0);
        REQUIRE(b == 2);
    }
}
TEST_CASE("graph_adjacency_contraction_nonsimple", "[composite]") {
    std::set<std::tuple<int, int>> s;
    s.emplace(0, 1);
    s.emplace(1, 2);
    s.emplace(1, 3);

    bool done;
    std::tie(s, done) = composite_with_leftovers(s, s);
    {
        REQUIRE(done == false);
        REQUIRE(s.size() == 2);
        auto it = s.begin();
        {
            auto [a, b] = *it++;
            REQUIRE(a == 0);
            REQUIRE(b == 2);
        }
        {
            auto [a, b] = *it++;
            REQUIRE(a == 0);
            REQUIRE(b == 3);
        }
    }
    std::tie(s, done) = composite_with_leftovers(s, s);
    {
        REQUIRE(done == true);
        REQUIRE(s.size() == 2);
        auto it = s.begin();
        {
            auto [a, b] = *it++;
            REQUIRE(a == 0);
            REQUIRE(b == 2);
        }
        {
            auto [a, b] = *it++;
            REQUIRE(a == 0);
            REQUIRE(b == 3);
        }
    }
}
*/
