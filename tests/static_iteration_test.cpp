#include <iostream>
#include <iterator>
#include "static_iteration.h"
#include <array>


struct PerDimTest {
    PerDimTest(const std::array<int,5>& a): m_data(a) {
    }
    using value_type = int;
    template <int D>
    int& get() {
        return m_data[D];
    }
    template <int D>
    int get() const {
        return m_data[D];
    }
    const std::array<int,5>& data() const {return m_data;}

    private:
        std::array<int,5> m_data;
};
struct PerDimTest2 {
    using value_type = void;
    template <int D>
    void run() const {
        std::cout << D << std::endl;
    }
};

int main(int argc, char * argv[]) {
    PerDimTest a{{{1,2,3,4,5}}};
    mtao::static_loop<5>([](int I,int& b) {
            b = b * 4;
            },a);
    std::copy(a.data().begin(),a.data().end(),std::ostream_iterator<int>(std::cout," "));
    std::cout << std::endl;
    mtao::const_static_loop<5>([](int I, int b) {
            std::cout << b << " ";
            },a);
    std::cout << std::endl;

    PerDimTest2 pdt2;
    mtao::static_loop_void<20>(pdt2);
}
