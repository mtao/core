#ifndef SCALAR_EXAMPLE_FUNC_H
#define SCALAR_EXAMPLE_FUNC_H
#include "scalar_function.h"
#include <Eigen/Dense>
#include <memory>
#include <Eigen/Geometry>
#include <cmath>
#include "coloring.h"
#include <iostream>









template <int _D>
class ScalarFunctionWithId: public ScalarFunction<_D> {
    public:
        USE_BASE_SCALAR_FUNCTION_DEFS(ScalarFunction)
            ScalarFunctionWithId(): id(counter++) {
            }
    private:

        static int counter = 0;
        int id = 0;
};
template <int D>
int ScalarFunctionWithId<D>::counter = 0;











namespace examples {
    template <int D>
        std::shared_ptr<ScalarFunction<D> > translate(const typename ScalarFunction<D>::Ptr& f, int Dim) {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,D,1>;
            return std::make_shared<TranslatingScalarFunction<D>>(f,1.*Vec::Unit(Dim));
        }
    template <int D>
        std::shared_ptr<ScalarFunction<D> > rotation(const typename ScalarFunction<D>::Ptr& f);
    template <>
        std::shared_ptr<ScalarFunction<2> > rotation(const typename ScalarFunction<2>::Ptr& f) ;
    template <>
        std::shared_ptr<ScalarFunction<3> > rotation(const typename ScalarFunction<3>::Ptr& f) ;
    template <int D>
        std::shared_ptr<ScalarFunction<D> > sphere() {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,D,1>;
            return std::make_shared<SphereLevelset<D>>(Vec::Constant(.5),.3);
        }
    template <int D>
        std::shared_ptr<ScalarFunction<D> > blobby() {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,D,1>;
            return std::make_shared<BlobbyLevelset<D>>(Vec::Constant(.5),.3);
        }

    template <int D>
        std::shared_ptr<ScalarFunction<D> > ellipse() {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,D,1>;
            auto s = std::make_shared<SphereLevelset<D>>(Vec::Constant(.5),.3);
            return std::make_shared<ScalingScalarFunction<D> >(s,Vec::Constant(.5),Vec::LinSpaced(D,.8,1.));
        }


    template <int D>
        std::shared_ptr<ScalarFunction<D> > uncenteredEllipse() {
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
            auto r = std::make_shared<ScalingScalarFunction<D> >(s,Vec::Zero(),Vec::LinSpaced(D,.6,1.));
            return std::make_shared<TranslateScalarFunction<D> >(r,rp(0));

        }
    template <int D>
        std::shared_ptr<ScalarFunction<D> > uncenteredSphere() {
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
            return std::make_shared<TranslateScalarFunction<D> >(sphere1,Vec::Unit(1)*.3);
            return sphere1;

        }
    template <int D>
        std::shared_ptr<ScalarFunction<D> > uncenteredBlobby() {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,D,1>;
            using Vec2 = Eigen::Matrix<Scalar,2,1>;
            Scalar ang = 2 * 3.1415967 / 3;
            auto rp = [&](Scalar a) -> Vec {
                Vec v = Vec::Constant(.5);
                v.template topRows<2>() += .1 * Vec2(cos(a*ang),sin(a*ang));
                return v;
            };
            auto sphere1 = std::make_shared< BlobbyLevelset<D> >(rp(0),.2);
            return std::make_shared<SumScalarFunction<D> >(sphere1);

        }
    template <int D>
        std::shared_ptr<ScalarFunction<D> > threeBlobbies() {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,D,1>;
            using Vec2 = Eigen::Matrix<Scalar,2,1>;
            Scalar ang = 2 * 3.1415967 / 3;
            auto rp = [&](Scalar a) -> Vec {
                Vec v = Vec::Constant(.5);
                v.template topRows<2>() += .1 * Vec2(cos(a*ang),sin(a*ang));
                return v;
            };
            auto sphere1 = std::make_shared< BlobbyLevelset<D> >(rp(0),.2);
            auto sphere2 = std::make_shared< BlobbyLevelset<D> >(rp(1),.2);
            auto sphere3 = std::make_shared< BlobbyLevelset<D> >(rp(2),.2);
            //                auto sphere4 = std::make_shared< SphereLevelset<2> >(Vec2(.5,.5),.2);
            //return std::make_shared<MinScalarFunction<D> >(sphere1,sphere2,sphere3);
            return std::make_shared<SumScalarFunction<D> >(sphere1,sphere2,sphere3);

        }
    template <int D>
        std::shared_ptr<ScalarFunction<D> > threeSpheres() {
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
            return std::make_shared<MinScalarFunction<D> >(sphere1,sphere2,sphere3);

        }





}








#endif//SCALAR_EXAMPLE_FUNC_H
