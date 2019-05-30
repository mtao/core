#include <mtao/type_utils.h>
#include <iostream>
#include <vector>
int main() {

    std::vector<int> a(20);
    std::cout << mtao::types::container_size(a) << " = " << a.size() << std::endl;
    int b[15];
    std::cout << mtao::types::container_size(b) << " = " << 15 << std::endl;

}
