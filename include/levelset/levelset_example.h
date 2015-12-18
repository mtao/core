#ifndef LEVELSET_EXAMPLE_FUNC_H
#define LEVELSET_EXAMPLE_FUNC_H
#include <Eigen/Dense>
#include <memory>
#include <Eigen/Geometry>
#include <cmath>

#include "levelset.h"
#include "primitives.h"
#include "transformations.h"
#include "csg.h"








/*
template <int _D>
class LevelsetWithId: public Levelset<_D> {
    public:
        USE_BASE_LEVELSET_FUNCTION_DEFS(Levelset)
            LevelsetWithId(): id(counter++) {
            }
    private:

        static int counter = 0;
        int id = 0;
};
template <int D>
int LevelsetWithId<D>::counter = 0;
*/











namespace levelset {

    template <int D>
        std::shared_ptr<Levelset<D> > ellipse() {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,D,1>;
            auto s = std::make_shared<SphereLevelset<D>>(Vec::Constant(.5),.3);
            return std::make_shared<LevelsetScaler<D> >(s,Vec::Constant(.5),Vec::LinSpaced(D,.8,1.));
        }


    template <int D>
        std::shared_ptr<Levelset<D> > uncenteredEllipse() {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,D,1>;
            using Vec2 = Eigen::Matrix<Scalar,2,1>;
            Scalar ang = 2 * 3.1415967 / 3;
            auto rp = [&](Scalar a) -> Vec {
                Vec v = Vec::Constant(.5);
                v.template topRows<2>() += -.3 * Vec2(cos(a*ang),sin(a*ang));
                return v;
            };
            auto s = std::make_shared<SphereLevelset<D>>(Vec::Zero(),.2);
            auto r = std::make_shared<LevelsetScaler<D> >(s,Vec::Zero(),Vec::LinSpaced(D,.6,1.));
            return std::make_shared<LevelsetTranslator<D> >(r,rp(0));

        }
    template <int D>
        std::shared_ptr<Levelset<D> > uncenteredSphere() {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,D,1>;
            using Vec2 = Eigen::Matrix<Scalar,2,1>;
            Scalar ang = 2 * 3.1415967 / 3;
            auto rp = [&](Scalar a) -> Vec {
                Vec v = Vec::Constant(.5);
                v.template topRows<2>() += -.3 * Vec2(cos(a*ang),sin(a*ang));
                return v;
            };
//            auto sphere1 = std::make_shared< BlobbyLevelset<D> >(rp(0),.2);
            auto sphere1 = std::make_shared< SphereLevelset<D> >(Vec::Zero(),.2);
            return std::make_shared<LevelsetTranslator<D> >(sphere1,Vec::Unit(1)*.3);
            return sphere1;

        }
    template <int D>
        std::shared_ptr<Levelset<D> > threeSpheres() {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,D,1>;
            using Vec2 = Eigen::Matrix<Scalar,2,1>;
            Scalar ang = 2 * 3.1415967 / 3;
            auto rp = [&](Scalar a) -> Vec {
                Vec v = Vec::Constant(.5);
                v.template topRows<2>() += .1 * Vec2(cos(a*ang),sin(a*ang));
                return v;
            };
            auto sphere1 = std::make_shared< SphereLevelset<D> >(rp(0),.2);
            auto sphere2 = std::make_shared< SphereLevelset<D> >(rp(1),.2);
            auto sphere3 = std::make_shared< SphereLevelset<D> >(rp(2),.2);
            //                auto sphere4 = std::make_shared< SphereLevelset<2> >(Vec2(.5,.5),.2);
            return std::make_shared<TernaryCSGUnion<D> >(sphere1,sphere2,sphere3);

        }





}








#endif//LEVELSET_EXAMPLE_FUNC_H
