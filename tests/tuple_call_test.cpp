#include <iostream>
#include <mtao/functional.hpp>

auto f(int a, double b) {
    return a * b;
}
std::string g() {
    return "asdf";
}

int main(int argc, char * argv[]) {
    std::cout << mtao::functional::evaluate_tuple(f,std::make_tuple(2,3)) << std::endl;
    std::cout << mtao::functional::evaluate_tuple(g) << std::endl;
    return 0;
}
