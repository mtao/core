#include "mtao/geometry/mesh/read_plc.hpp"
#include "mtao/geometry/mesh/write_plc.hpp"


int main() {
    mtao::ColVectors<float,2> V(2,20);
    for(int i = 0; i < V.cols(); ++i) {
        V.col(i) = mtao::Vector<float,2>(2.0 * i, i.0);
    }
}
