#include <iostream>
#include "levelset/levelset_example.h"
#include "levelset/levelset.h"


void translateTest() {
    auto sphere = examples::sphere<2>();
    std::shared_ptr<Levelset<2>> transphere = examples::translate<2>(sphere,0);
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

    auto zls = ZeroLevelset<2>();

}
