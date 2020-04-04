#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/reindex/compressing_reindexer.hpp>
#include <mtao/reindex/offset_reindexer.hpp>
#include <mtao/reindex/stacked_reindexer.hpp>
#include <mtao/reindexer.hpp>

TEST_CASE("CompressingReindexer", "[reindexer]") {
    mtao::reindex::CompressingReindexer<size_t> a;
    a.add(5);
    a.add(10);
    a.add(12);
    a.add(10);
    REQUIRE(a.index(5) == size_t(0));
    REQUIRE(a.index(10) == size_t(1));
    REQUIRE(a.index(12) == size_t(2));
    REQUIRE(a.add(11) == size_t(3));
    REQUIRE(a.add(5) == size_t(0));

    {
        mtao::reindex::CompressingReindexer<std::tuple<int, int>> a;
        a.add(0, 5);
        a.add(0, 10);
        a.add(0, 12);
        a.add(0, 10);
        REQUIRE(a.index(0, 5) == size_t(0));
        REQUIRE(a.index(0, 10) == size_t(1));
        REQUIRE(a.index(0, 12) == size_t(2));
        REQUIRE(a.add(2, 11) == size_t(3));
        REQUIRE(a.add(0, 5) == size_t(0));
    }
}
TEST_CASE("OffsetReindexer", "[reindexer]") {
    mtao::reindex::OffsetReindexer<size_t> a;
    a.set_size(20);
    a.set_offset(30);
    REQUIRE(a.index(0) == size_t(30));
    REQUIRE(a.index(5) == size_t(35));

    {
        mtao::reindex::StackedReindexer<int> a({20, 10, 2, 30, 5});
        REQUIRE(a.index(0, 5) == size_t(5));
        REQUIRE(a.index(0, 10) == size_t(10));
        REQUIRE(a.index(0, 12) == size_t(12));
        REQUIRE(a.index(1, 9) == size_t(29));
        REQUIRE(a.index(2, 1) == size_t(31));
        REQUIRE(a.index(3, 5) == size_t(37));
        REQUIRE(a.index(4, 4) == size_t(66));

        REQUIRE(std::pair<size_t, int>(0, 5) == a.unindex(size_t(5)));
        REQUIRE(std::pair<size_t, int>(0, 10) == a.unindex(size_t(10)));
        REQUIRE(std::pair<size_t, int>(0, 12) == a.unindex(size_t(12)));
        REQUIRE(std::pair<size_t, int>(1, 9) == a.unindex(size_t(29)));
        REQUIRE(std::pair<size_t, int>(2, 1) == a.unindex(size_t(31)));
        REQUIRE(std::pair<size_t, int>(3, 5) == a.unindex(size_t(37)));
        REQUIRE(std::pair<size_t, int>(4, 4) == a.unindex(size_t(66)));
        REQUIRE(size_t(5) == a.index(a.unindex(size_t(5))));
        REQUIRE(size_t(10) == a.index(a.unindex(size_t(10))));
        REQUIRE(size_t(12) == a.index(a.unindex(size_t(12))));
        REQUIRE(size_t(29) == a.index(a.unindex(size_t(29))));
        REQUIRE(size_t(31) == a.index(a.unindex(size_t(31))));
        REQUIRE(size_t(37) == a.index(a.unindex(size_t(37))));
        REQUIRE(size_t(66) == a.index(a.unindex(size_t(66))));
    }
}

/*
int main(int argc, char * argv[]) {

    mtao::ReIndexer<int> reidx;
    mtao::StackedReIndexer<int,3> sreidx;
    for(auto&& k: sreidx.offsets) {
        std::cout << k << ",";
    }
    std::cout << std::endl;
    for(int i = 0; i < 5; ++i) {
        reidx.add_index(1<<i);
        sreidx.add_index<0>(1<<i);
        sreidx.add_index<1>(3*i);
        sreidx.add_index<2>(i);
    }
    for(auto&& [k,v]: reidx.map()) {
        std::cout << k << "=>" << v << std::endl;
    }
    for(auto&& k: reidx.inverse()) {
        std::cout << k << ",";
    }
    std::cout << std::endl;

    for(auto&& i: sreidx.indexers) {
        std::cout << i.size() << ":";
    }
    std::cout << std::endl;
    for(auto&& i: sreidx.offsets) {
        std::cout << i << ":";
    }
    std::cout << std::endl;
    for(auto&& k: sreidx.inverse()) {
        std::cout << k << ",";
    }
    std::cout << std::endl;

    return 0;
}
*/
