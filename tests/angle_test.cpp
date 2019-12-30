#include <iostream>
#include <mtao/geometry/trigonometry.hpp>
#include <catch2/catch.hpp>

using namespace mtao::geometry::trigonometry;

TEST_CASE("Polygon angles", "[trigonometry]") {
    mtao::ColVecs2d V(2,5);
    V.col(0) = mtao::Vec2d(0.0,0.0);
    V.col(1) = mtao::Vec2d(1.0,0.0);
    V.col(2) = mtao::Vec2d(1.0,1.0);
    V.col(3) = mtao::Vec2d(0.0,1.0);
    V.col(4) = mtao::Vec2d(0.5,0.5);


    
    for(int n = 3; n < 5; ++n) {
        std::stringstream ss;
        ss << "n = " << n;
        SECTION( ss.str().c_str() ) {
            std::vector<int> a(n);
            std::iota(a.begin(),a.end(),0);
            std::vector<int> b(n);
            std::copy(a.rbegin(),a.rend(),b.begin());
            REQUIRE( Approx(M_PI * 2 * n) == interior_angle_sum(n) + exterior_angle_sum(n));
            REQUIRE( angle_sum(V,a) == Approx(interior_angle_sum(n)));
            REQUIRE( angle_sum(V,b) == Approx(exterior_angle_sum(n)));
        }
    }

}
