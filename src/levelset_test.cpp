#include <iostream>
#include "levelset/levelset_example.h"
#include "levelset/levelset.h"

void unionTest() {
    auto sphere = levelset::sphere<2>();
    auto cube = levelset::cube<2>();
    auto transphere = levelset::axis_translation<2>(sphere,0);
    auto trancube = levelset::axis_translation<2>(cube,1);
    auto u = levelset::csg_union<2>(transphere,trancube);
    using Vec = Eigen::Vector2f;
    int num_reps = 20;
    for(int i = 0; i < 20; ++i) {
        float t = float(i)/num_reps;
        Vec center = Vec::Zero();
        Vec true_center = Vec::Zero();
        true_center(0) = t;
        Vec true_center2 = Vec::Zero();
        true_center2(1) = -t;
        std::cout << 
        (*u)(center,t) << " sphere:" <<
        (*u)(true_center,t) << " cube:" <<
        (*u)(true_center2,t) << std::endl;

    }
}

void translateTest() {
    //auto sphere = levelset::sphere<2>();
    auto sphere = levelset::cube<2>();
    auto transphere = levelset::axis_translation<2>(sphere,0);
    using Vec = Eigen::Vector2f;
    int num_reps = 20;
    for(int i = 0; i < 20; ++i) {
        float t = float(i)/num_reps;
        Vec center = Vec::Zero();
        Vec true_center = Vec::Zero();
        true_center(0) = t;
        std::cout << 
        (*transphere)(center,t) << " " <<
        (*transphere)(true_center,t) << std::endl;

    }
}


int main( int argc, char * argv[]) {
    translateTest();
    unionTest();

    auto zls = levelset::ZeroLevelset<2>();

}
