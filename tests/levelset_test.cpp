#include <iostream>
#include "mtao/geometry/levelset/levelset_example.h"
#include "mtao/geometry/levelset/levelset.h"
#include "mtao/geometry/levelset/discrete.h"
#include "mtao/geometry/levelset/discrete/print.h"

void unionTest() {
    auto sphere = levelset::sphere<2>();
    auto cube = levelset::cube<2>();
    auto transphere = levelset::axis_translation<2>(sphere, 0);
    auto trancube = levelset::axis_translation<2>(cube, 1);
    auto u = levelset::csg_union<2>(transphere, trancube);
    using Vec = Eigen::Vector2f;
    int num_reps = 20;
    for (int i = 0; i < 20; ++i) {
        float t = float(i) / num_reps;
        Vec center = Vec::Zero();
        Vec true_center = Vec::Zero();
        true_center(0) = t;
        Vec true_center2 = Vec::Zero();
        true_center2(1) = -t;
        std::cout << (*u)(center, t) << " sphere:" << (*u)(true_center, t) << " cube:" << (*u)(true_center2, t) << std::endl;
    }
}


void translateTest() {
    //auto sphere = levelset::sphere<2>();
    auto sphere = levelset::cube<2>();
    auto transphere = levelset::axis_translation<2>(sphere, 0);
    using Vec = Eigen::Vector2f;
    int num_reps = 20;
    for (int i = 0; i < 20; ++i) {
        float t = float(i) / num_reps;
        Vec center = Vec::Zero();
        Vec true_center = Vec::Zero();
        true_center(0) = t;
        std::cout << (*transphere)(center, t) << " " << (*transphere)(true_center, t) << std::endl;
    }
}

void discreteTest() {
    auto sphere = levelset::sphere<2>();
    auto cube = levelset::cube<2>();
    auto transphere = levelset::axis_translation<2>(sphere, 0);
    auto trancube = levelset::enstatic(levelset::axis_translation<2>(cube, 1));
    auto rotcube = levelset::rotation<2>(trancube);
    auto u = levelset::csg_union<2>(transphere, rotcube);
    using DDLPTR = std::remove_reference<decltype(*levelset::dense_discrete<2>(transphere, 1))>::type;
    using BB = DDLPTR::BBox;
    using Vec = DDLPTR::Vec;
    BB a(Vec::Constant(-3), Vec::Constant(3));
    auto dd = levelset::dense_discrete<2>(u, 100, 2, a);

    for (auto &&g : dd->grids()) {
        levelset::discrete::print_grid(g);
    }
}
void squishTest() {
    auto sphere = levelset::sphere<2>();
    using DDLPTR = std::remove_reference<decltype(*levelset::dense_discrete<2>(sphere, 1))>::type;
    using BB = DDLPTR::BBox;
    //using Vec = DDLPTR::Vec;
    using Vec = Eigen::Vector2f;
    auto squish1 = levelset::scaling<2>(sphere, Vec(0, 0), Vec(1., 2));
    auto squish2 = levelset::scaling<2>(squish1, Vec(0, 0), Vec(.5, 1));
    BB a(Vec::Constant(-3), Vec::Constant(3));
    auto dd = levelset::dense_discrete<2>(squish2, 100, 2, a);

    for (auto &&g : dd->grids()) {
        levelset::discrete::print_grid(g);
    }
}


int main(int argc, char *argv[]) {
    //translateTest();
    //unionTest();
    //discreteTest();
    squishTest();


    auto zls = levelset::ZeroLevelset<2>();
}
