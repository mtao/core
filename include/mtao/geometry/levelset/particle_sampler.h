#ifndef PARTICLE_SAMPLER_H
#define PARTICLE_SAMPLER_H
#include "grid.h"
#include <Eigen/Dense>
#include <vector>


template<typename Scalar_, int D_>
class ParticleSampler {
  public:
    using Scalar = Scalar_;
    constexpr static int D = D_;
    using GridType = mtao::Grid<Scalar, D>;
    using IndexType = typename GridType::index_type;
    using VecType = Eigen::Matrix<Scalar, D, 1>;
    using VecVector = std::vector<VecType, Eigen::aligned_allocator<VecType>>;

    const VecVector &run(const GridType &g) {
        setDX(g.shape());
        IndexType shape = g.shape();
        run_(shape, g, type<0>());
        return points();
    }
    const VecVector &points() const { return m_points; }

  private:
    void setDX(const IndexType &index) {
        using IArr = Eigen::Array<typename IndexType::value_type, D, 1>;
        m_dx = 1 / (Eigen::Map<const IArr>(index.begin()).template cast<Scalar>() - 1);
    }
    VecType index2world(const IndexType &idx) const {
        using IVec = Eigen::Matrix<typename IndexType::value_type, D, 1>;
        return Eigen::Map<const IVec>(idx.begin()).template cast<Scalar>().cwiseProduct(m_dx);
    }


    template<int D>
    struct type {};

    template<int I = 0>
    void run_(IndexType &shape, const GridType &g, type<I>) {
        shape[I]--;
        mtao::multi_loop(shape, [&](auto idx) {
            auto &&a = g(idx);
            idx[I]++;
            auto &&b = g(idx);
            if (a * b <= 0) {
                VecType v2 = this->index2world(idx);
                Scalar theta = b / (b - a);
                v2[I] -= m_dx[I] * theta;
                m_points.push_back(v2);
            }
        });
        shape[I]++;
        run_(shape, g, type<I + 1>());
    }
    void run_(IndexType &shape, const GridType &g, type<D>) {}
    VecType m_dx;
    VecVector m_points;
};

template<typename Scalar, int D>
//std::vector<Eigen::Matrix<Scalar,D,1>,Eigen::aligned_allocator<Eigen::Matrix<Scalar,D,1>> > getParticles(const mtao::Grid<
auto getParticles(const mtao::Grid<Scalar, D> &g) {
    ParticleSampler<Scalar, D> ps;
    return ps.run(g);
}


#endif//PARTICLE_SAMPLER_H
