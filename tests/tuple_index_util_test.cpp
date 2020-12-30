#include <iostream>
#include <mtao/type_utils.hpp>
#include <catch2/catch.hpp>

using namespace mtao::types;

TEST_CASE("tuple_element_index", "[types,tuple]") {
    REQUIRE(tuple_element_index_v<int, std::tuple<int, char>> == 0);
    REQUIRE(tuple_element_index_v<char, std::tuple<int, char>> == 1);

    REQUIRE(tuple_element_index_v<std::string, std::tuple<int, char, std::string>> == 2);
}
