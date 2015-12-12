#include "scalar_function_examples.h"
#include "sphere.hpp"
#include <iostream>



namespace internal_scalar_function_examples {
    using Vec = SphereLevelset<3>::Vec;
    using VecVector = SphereLevelset<3>::VecVector;
    /*

       static constexpr VecVector tet_verts({
       Vec(1,1,1),
       Vec(1,-1,-1),
       Vec(-1,1,-1),
       Vec(-1,-1,1),
       });
       VecVector sphereVertices(
       */

}

namespace examples {
    template <>
        std::shared_ptr<ScalarFunction<2> > rotation(const typename ScalarFunction<2>::Ptr& f) {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,2,1>;
            return std::make_shared<RotatingScalarFunction<2>>(f,Vec::Constant(.5),1);
        }
    template <>
        std::shared_ptr<ScalarFunction<3> > rotation(const typename ScalarFunction<3>::Ptr& f) {
            using Scalar = float; 
            using Vec = Eigen::Matrix<Scalar,3,1>;
            return std::make_shared<RotatingScalarFunction<3>>(f,Vec::Constant(.5),Vec::Unit(2),1);
        }
}
