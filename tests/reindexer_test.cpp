#include <mtao/reindexer.hpp>
#include <iostream>


int main(int argc, char * argv[]) {

    mtao::ReIndexer<int> reidx;
    mtao::StackedReIndexer<int,3> sreidx;
    for(auto&& k: sreidx.offsets) {
        std::cout << k << ",";
    }
    std::cout << std::endl;
    for(int i = 0; i < 5; ++i) {
        reidx.add_index(1<<i);
        sreidx.add_index<0>(1<<i);
        sreidx.add_index<1>(3*i);
        sreidx.add_index<2>(i);
    }
    for(auto&& [k,v]: reidx.map()) {
        std::cout << k << "=>" << v << std::endl;
    }
    for(auto&& k: reidx.inverse()) {
        std::cout << k << ",";
    }
    std::cout << std::endl;
    
    for(auto&& i: sreidx.indexers) {
        std::cout << i.size() << ":";
    }
    std::cout << std::endl;
    for(auto&& i: sreidx.offsets) {
        std::cout << i << ":";
    }
    std::cout << std::endl;
    for(auto&& k: sreidx.inverse()) {
        std::cout << k << ",";
    }
    std::cout << std::endl;

    return 0;
}
