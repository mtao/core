#include <iostream>
#include <mtao/copyable_unique_ptr.hpp>


int main() {
    mtao::copyable_unique_ptr<int> a;
    mtao::copyable_unique_ptr<int> b = mtao::make_copyable_unique_ptr(3);
    a = b;
    std::cout << *a << " = " << *b << std::endl;
}
