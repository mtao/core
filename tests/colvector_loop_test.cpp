#include "mtao/colvector_loop.hpp"
#include "mtao/logging/timer.hpp"
#include <iostream>



int main() {
    mtao::ColVectors<float,2> P(2,20);
    for(auto&& p: mtao::colvector_loop(P)) {
        p = mtao::Vector<float,2>(2.0,3.0);
    }
    const mtao::ColVectors<float,2>& Q = P;
    for(auto&& p: mtao::colvector_loop(Q)) {
        std::cout << p.transpose() << std::endl;
    }

}

