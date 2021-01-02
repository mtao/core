#ifndef MTAO_VDB_CONVERSION_H
#define MTAO_VDB_CONVERSION_H
#include <Eigen/Dense>
#include <openvdb/math/Vec2.h>
#include <openvdb/math/Vec3.h>
#include <openvdb/math/Vec4.h>

namespace mtao {
namespace internal {
    //openvdb selector based on dim and scalar type
    template<int Dim>
    struct vdb_selector {};
    template<>
    struct vdb_selector<2> {
        template<typename Scalar>
        struct Vec {
            typedef typename openvdb::math::Vec2<Scalar> type;
        };
    };
    template<>
    struct vdb_selector<3> {
        template<typename Scalar>
        struct Vec {
            typedef typename openvdb::math::Vec3<Scalar> type;
        };
    };
    template<>
    struct vdb_selector<4> {
        template<typename Scalar>
        struct Vec {
            typedef typename openvdb::math::Vec4<Scalar> type;
        };
    };
    //method to extract the type of a vdb grid
    template<template<class> class VecType>
    struct vdb_dim {};
    template<>
    struct vdb_dim<openvdb::math::Vec2> {
        enum { value = 2 };
    };
    template<>
    struct vdb_dim<openvdb::math::Vec3> {
        enum { value = 3 };
    };
    template<>
    struct vdb_dim<openvdb::math::Vec4> {
        enum { value = 4 };
    };
    template<typename VecType>
    struct vdb_traits {};

    template<template<class> class VecType, typename Scalar_>
    struct vdb_traits<VecType<Scalar_>> {
        typedef Scalar_ Scalar;
        enum { dim = vdb_dim<VecType>::value };
    };

}// namespace internal
}// namespace mtao

//The worst spaghetti code ever :)
//Basically we select the dimension and type of a vector individually through selection structs.
template<int Dim, typename Scalar>
typename mtao::internal::vdb_selector<Dim>::template Vec<Scalar>::type eigen2vdb(const Eigen::Matrix<Scalar, Dim, 1> &v) {
    return typename mtao::internal::vdb_selector<Dim>::template Vec<Scalar>::type(v.data());
}
//Eigen can do maps, which is better than a full copy :)
template<typename VecType>
Eigen::Map<const Eigen::Matrix<typename mtao::internal::vdb_traits<VecType>::Scalar, mtao::internal::vdb_traits<VecType>::dim, 1>> vdb2eigen(const VecType &v) {
    return Eigen::Map<const Eigen::Matrix<typename mtao::internal::vdb_traits<VecType>::Scalar, mtao::internal::vdb_traits<VecType>::dim, 1>>(v.asV());
}


#endif
