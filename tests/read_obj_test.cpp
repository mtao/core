#include <catch2/catch.hpp>
#include <iostream>
#include <mtao/geometry/mesh/read_obj.hpp>
#include <sstream>

using namespace mtao::geometry::mesh;

TEST_CASE("Read obj", "[mesh_io]") {
    {
        std::istringstream ss(
          "v 0 1 2\n"
          "v 3 4 5\n"
          "\n"
          "f 103 4/2 5\n"
          "f 106 7 8\n"
          "f 109/1 10/4/4 11\n");

        auto [V, F, E] = read_objF(ss);
        REQUIRE(V.size() == 6);
        REQUIRE(F.size() == 9);
        REQUIRE(V(0, 0) == 0);
        REQUIRE(V(1, 0) == 1);
        REQUIRE(V(2, 0) == 2);
        REQUIRE(V(0, 1) == 3);
        REQUIRE(V(1, 1) == 4);
        REQUIRE(V(2, 1) == 5);

        REQUIRE(F(0, 0) == 102);
        REQUIRE(F(1, 0) == 3);
        REQUIRE(F(2, 0) == 4);
        REQUIRE(F(0, 1) == 105);
        REQUIRE(F(1, 1) == 6);
        REQUIRE(F(2, 1) == 7);
        REQUIRE(F(0, 2) == 108);
        REQUIRE(F(1, 2) == 9);
        REQUIRE(F(2, 2) == 10);
    }
    {
        std::istringstream ss("f 103 1 2 3\n");

        REQUIRE_THROWS(read_objF(ss));
        //REQUIRE(V.size() == 0);
        //REQUIRE(F.size() == 6);

        //REQUIRE(F(0, 0) == 102);
        //REQUIRE(F(1, 0) == 0);
        //REQUIRE(F(2, 0) == 1);
        //REQUIRE(F(0, 1) == 102);
        //REQUIRE(F(1, 1) == 1);
        //REQUIRE(F(2, 1) == 2);
    }

    {
        std::istringstream ss(
          "v 0 1\n"
          "v 3 4\n"
          "\n"
          "#234\n"
          "vf 3 4 5\n"
          "e 103 4/2\n"
          "e 106 7\n"
          "e 109/1 10/4/4\n");

        auto [V, F] = read_obj2F(ss);
        REQUIRE(V.size() == 4);
        REQUIRE(F.size() == 6);
        REQUIRE(V(0, 0) == 0);
        REQUIRE(V(1, 0) == 1);
        REQUIRE(V(0, 1) == 3);
        REQUIRE(V(1, 1) == 4);

        REQUIRE(F(0, 0) == 102);
        REQUIRE(F(1, 0) == 3);
        REQUIRE(F(0, 1) == 105);
        REQUIRE(F(1, 1) == 6);
        REQUIRE(F(0, 2) == 108);
        REQUIRE(F(1, 2) == 9);
    }
}
