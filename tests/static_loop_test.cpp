#include <iostream>
#include <mtao/static_iteration.h>


template <int D>
struct f {
    int operator()() {
    return 2*D;
    }
};

int main() {
    mtao::static_loop<5>([&](int idx){
            std::cout << idx << std::endl;
            });
    auto [a,b,c,d,e]= mtao::static_loop<5>([&](int idx) {
            return idx;
            });
    std::cout << a << "," << b <<"," << c <<"," << d <<"," << e << std::endl;
}
