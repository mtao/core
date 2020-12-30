#include <mtao/geometry/mesh/triangle_fan.hpp>
#include <mtao/geometry/mesh/earclipping.hpp>


int main() {


    std::vector<int> F{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };

    std::cout << mtao::geometry::mesh::triangle_fan(F) << std::endl;

    return 0;
}
