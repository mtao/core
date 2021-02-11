#pragma once
#include "mtao/types.hpp"
#include <openvdb/Types.h>

namespace mtao::geometry::point_cloud {
// for use with openvdb::tools::ParticlesToLevelSet
struct VDBParticleList_Pos {
    mtao::ColVecs3d P;

    size_t size() const {
        return P.cols();
    }
    void getPos(size_t n, openvdb::Vec3R &xyz) const {
        auto p = P.col(n);
        xyz = openvdb::Vec3R(p.x(), p.y(), p.z());
    }
};
struct VDBParticleList_PosRad : public VDBParticleList_Pos {
    mtao::VecXd R;
    void getPosRad(size_t n, openvdb::Vec3R &xyz, openvdb::Real &radius) const {
        getPos(n, xyz);
        radius = R(n);
    }
};
struct VDBParticleList_PosRadVec : public VDBParticleList_PosRad {
    VDBParticleList_PosRadVec(const mtao::ColVecs3d
    void getPosRadVel(size_t n, openvdb::Vec3R &xyz, openvdb::Real &radius, openvdb::Vec3R &vel) const {
        getPosRad(n, xyz, radius);
        auto v = V.col(n);
        vel = openvdb::Vec3R(v.x(), v.y(), v.z());
    }
    mtao::ColVecs3d V;
};
};

}// namespace mtao::geometry::point_cloud
