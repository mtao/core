#include <iostream>
#include <mtao/geometry/kdtree.hpp>
#include <algorithm>
using namespace mtao::geometry;

int main(int argc, char* argv[]) {
    KDTree<double,2> kdt;
    mtao::vector<mtao::Vector<double,2>> vec(1000);

    std::generate(vec.begin(),vec.end(),[]() { return mtao::Vector<double,2>::Random().eval(); });
    for(auto&& v: vec) {
        kdt.insert(v);
        //std::cout << kdt.max_depth() << ", ";
        //std::cout << double(kdt.max_depth()) / kdt.size() << std::endl;
    }
    std::cout << kdt.max_depth() << ", ";
    std::cout << double(kdt.max_depth()) / kdt.size() << std::endl;

    kdt = KDTree<double,2>(vec);
    std::cout << kdt.max_depth() << ", ";
    std::cout << double(kdt.max_depth()) / kdt.size() << std::endl;
    std::cout << std::endl << std::endl;


    for(int i = 0; i < 1000; ++i) {
        auto v = mtao::Vector<double,2>::Random().eval();
        size_t idx = 0;
        {
            auto d = [&](size_t idx) {
                return (v - kdt.point(idx)).norm();
            };
            double dist =d(0);
                for(size_t i = 0; i < kdt.points().size(); ++i) {
                    double dst = d(i);
                    if(dst < dist) {
                        idx = i;
                        dist = dst;
                    }
                }
        }
        std::cout << "Size: " <<kdt.size() << std::endl;
        std::cout << std::string(kdt) << std::endl;
        size_t kdti =  kdt.nearest_index(v);
        if(idx !=kdti) {
            std::cout << "FAIL!" << std::endl;
            std::cout << idx << "," << kdti << std::endl; 
        } else {
            //std::cout << "Scucess!" << std::endl;
        }

    }
    std::cout << std::string(kdt) << std::endl;
    {
        KDTree<double,1> kdt;
        kdt.insert(mtao::Vector<double,1>(.5));
        kdt.insert(mtao::Vector<double,1>(.25));
        kdt.insert(mtao::Vector<double,1>(.75));
        kdt.insert(mtao::Vector<double,1>(.125));
        kdt.insert(mtao::Vector<double,1>(.375));
        kdt.insert(mtao::Vector<double,1>(.625));
        kdt.insert(mtao::Vector<double,1>(.875));
        std::cout << std::string(kdt) << std::endl;

        kdt.nearest(mtao::Vector<double,1>(.26));

    }

}
