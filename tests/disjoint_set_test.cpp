#include <iostream>
#include "mtao/data_structures/disjoint_set.hpp"

using namespace mtao::data_structures;
DisjointSet<int> a;


void print() {

    for(auto&& n: a.nodes) {
        if(n.root()) {
            std::cout << "Root: " << n.data << std::endl;
        } else {
            std::cout << "Internal: " << n.data << " ==> " << *n.parent << std::endl;
        }
    }
}

int main(int argc, char * argv[]) {
    a.add_node(0);
    a.add_node(1);
    a.add_node(2);
    a.add_node(3);
    a.add_node(4);
    a.add_node(5);
    a.add_node(6);
    a.add_node(7);



    a.join(0,1);
    a.join(0,2);
    a.join(0,3);
    a.join(0,4);
    a.join(6,1);
    a.join(5,7);
    a.reduce_all();
    print();
    for(auto&& i: a.root_indices()) {
        std::cout << a.node(i).data << ",";
    }
    std::cout << std::endl;

}

