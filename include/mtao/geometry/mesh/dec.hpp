#pragma once
#include <mtao/types.h>
#include <mtao/eigen_utils.h>
#include <Eigen/Sparse>
#include <boost/hana.hpp>

//Use DECMesh by inheriting it and manually implementing boundary / cell volumes / dual cell volumes / the cell counts per type
//
//CachedDECMesh caches the boundary computatoins / cell volume information for when it's expensive to compute these things
//
//DECMeshCore implements DEC h(hodge dual) d(exterior derivative) cod(codifferential operator as well as
//  the various laplacians (d cod / cod d / Laplace deRham (cod d + d cod) / weak laplacian (down) d cod h / weak laplacian (up) h cod d )
//  Note: standard cotan laplacian is weak_laplacian_up<0>()

namespace mtao { namespace geometry { namespace mesh {
template <typename Derived, typename T, int EmbeddedDim, int Dim_>
class DECMeshCore {
    public:
        using types = mtao::embedded_types<T,EmbeddedDim>;
        constexpr static int Dim = Dim_;
        using Scalar = typename types::Scalar;
        using VecX = typename types::VectorX;
        using SparseMatrix = Eigen::SparseMatrix<Scalar>;

        Derived& derived() { return *static_cast<Derived*>(this); }
        const Derived& derived() const { return *static_cast<const Derived*>(this); }

        template <int D>
        SparseMatrix boundary() const { return derived().template boundary<D>(); }
        template <int D>
        VecX volume() const { return derived().template volume<D>(); }
        template <int D>
        VecX dual_volume() const { return derived().template dual_volume<D>(); }

        //base operators
        template <int D>
        VecX h() const;
        template <int D>
        VecX hi() const;
        template <int D>
        SparseMatrix d() const;
        template <int D>
        SparseMatrix cod() const;

        template <int D>
        VecX h(const VecX& o) const { return h<D>().asDiagonal() * o; }
        //inverse hodge
        template <int D>
        VecX hi(const VecX& o) const { return hi<D>().asDiagonal() * o; }
        template <int D>
        VecX d(const VecX& o) const { return d<D>() * o; }
        //hi<D>dt<D-1>h<D>
        template <int D>
        VecX cod(const VecX& o) const { return cod<D>() * o; }

        //laplacians
        //d<D-1>hi<D>dt<D-1>h<D>
        template <int D>
        SparseMatrix laplacian_down() const;
        //hi<D>dt<D>h<D+1>d<D>
        template <int D>
        SparseMatrix laplacian_up() const;
        //d<D-1>hi<D>dt<D-1>h<D> + hi<D>dt<D>h<D+1>d<D>
        template <int D>
        SparseMatrix laplacian() const;
        //d<D-1>hi<D>dt<D-1>   (h<D>)
        template <int D>
        SparseMatrix weak_laplacian_down() const;
        //(hi<D>)    dt<D>h<D+1>d<D>
        template <int D>
        SparseMatrix weak_laplacian_up() const;

        template <int D>
        VecX weak_poisson_down(const VecX& o) const;
        template <int D>
        VecX weak_poisson_up(const VecX& o) const;
        template <int D>
        VecX weak_poisson_down_rhs(const VecX& o) const;
        template <int D>
        VecX weak_poisson_up_rhs(const VecX& o) const;

        //Find the ___ part according to HH decomposition
        //i.e o = da + hdhb + g

        //a
        template <int D>
        VecX coexact_down(const VecX& o) const ;
        //b
        template <int D>
        VecX exact_up(const VecX& o) const ;
        //da
        template <int D>
        VecX exact(const VecX& o) const ;
        //hdhb
        template <int D>
        VecX coexact(const VecX& o) const ;
        //da + g i.e o - hdhb
        template <int D>
        VecX closed(const VecX& o) const ;
        //hdhb + g i.e o - da
        template <int D>
        VecX coclosed(const VecX& o) const ;

};

template <typename Derived, typename T, int EmbeddedDim, int Dim_>
class DECMesh: public DECMeshCore<DECMesh<Derived, T, EmbeddedDim, Dim_>, T, EmbeddedDim, Dim_> {
    public:
        using types = mtao::embedded_types<T,EmbeddedDim>;
        constexpr static int Dim = Dim_;
        using Scalar = typename types::Scalar;
        using VecX = typename types::VectorX;
        using SparseMatrix = Eigen::SparseMatrix<Scalar>;
        using Mesh = DECMeshCore<DECMesh<Derived,T,EmbeddedDim,Dim>,T,EmbeddedDim,Dim>;

        Derived& derived() { return *static_cast<Derived*>(this); }
        const Derived& derived() const { return *static_cast<const Derived*>(this); }



        template <int D>
        SparseMatrix boundary() const { return derived().template boundary<D>(); }
        /*
        template <int C>
        auto volume(int i) const { return derived().template volume<C>(i); }
        template <int C>
        auto dual_volume(int i) const { return derived().template dual_volume<C>(i); }
        */
        template <int C>
        int cell_count() const { return derived().template cell_count<C>(); }
        template <int C>
        int dual_cell_count() const { return derived().template dual_cell_count<C>(); }
        template <int D>
        VecX volume() const { return derived().template volume<D>(); }
        template <int D>
        VecX dual_volume() const { return derived().template dual_volume<D>(); }
};
template <typename T, int EmbeddedDim, int Dim_>
class CachedDECMesh: public DECMesh<CachedDECMesh<T, EmbeddedDim,Dim_>, T, EmbeddedDim, Dim_> {
    public:
        using types = mtao::embedded_types<T,EmbeddedDim>;
        constexpr static int Dim = Dim_;
        using Scalar = typename types::Scalar;
        using VecX = typename types::VectorX;
        using SparseMatrix = Eigen::SparseMatrix<Scalar>;
        template <typename Derived>
            CachedDECMesh(const DECMesh<Derived,T,EmbeddedDim,Dim>& d) {
                using namespace boost;
                boost::hana::for_each(hana::make_range(hana::int_c<0>,hana::int_c<Dim+1>), [&](auto t) {
                        constexpr static int D = decltype(t)::value;
                        m_volume[D] = d.template volume<D>();
                        m_dual_volume[D] = d.template dual_volume<D>();
                        });
                boost::hana::for_each(hana::make_range(hana::int_c<1>,hana::int_c<Dim+1>), [&](auto t) {
                        constexpr static int D = decltype(t)::value;
                        m_boundary[D-1] = d.template boundary<D>();

                        });
            }


        template <int C>
        const SparseMatrix& boundary() const { return m_boundary[C-1]; }
        template <int C>
        auto volume(int i) const { return volume<C>()(i); }
        template <int C>
        auto dual_volume(int i) const { return dual_volume<C>()(i); }
        template <int C>
        int cell_count() const { return m_volume[C].rows();}
        template <int C>
        int dual_cell_count() const { return m_dual_volume[C].rows();}
        template <int C>
        const VecX& volume() const { return m_volume[C]; }
        template <int C>
        const VecX& dual_volume() const { return m_dual_volume[C]; }
    private:
        std::array<SparseMatrix, Dim> m_boundary;
        std::array<VecX, Dim+1> m_volume;
        std::array<VecX, Dim+1> m_dual_volume;

};

/*
template <typename Derived, typename T, int EmbeddedDim, int Dim>
template <int D>
auto DECMesh<Derived,T,EmbeddedDim,Dim>::volume() const -> VecX {
    int s = cell_count<D>();
    VecX v = VecX::Zero(s);
    for(int i = 0; i < s; ++i) {
        v(i) = volume<D>(i);
    }
    return v;
}
template <typename Derived, typename T, int EmbeddedDim, int Dim>
template <int D>
auto DECMesh<Derived,T,EmbeddedDim,Dim>::dual_volume() const -> VecX {
    int s = dual_cell_count<D>();
    VecX v = VecX::Zero(s);
    for(int i = 0; i < s; ++i) {
        v(i) = dual_volume<D>(i);
    }
    return v;
}
*/



//Base operators
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::h() const -> VecX {
    auto p = volume<D>();
    auto d = dual_volume<D>();

    VecX ret = d.cwiseQuotient(p);
    return mtao::eigen::finite(ret);
}
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::hi() const -> VecX {
    auto p = volume<D>();
    auto d = dual_volume<D>();

    VecX ret = p.cwiseQuotient(d);
    return mtao::eigen::finite(ret);
}
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::d() const -> SparseMatrix {
    return boundary<D+1>().transpose();
}

// * d *
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::cod() const -> SparseMatrix {
    return hi<D-1>().asDiagonal() * d<D-1>().transpose() * h<D>().asDiagonal();
}

//Laplacians
//d * d *
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>:: laplacian_down() const -> SparseMatrix {
    return d<D-1>() * cod<D>();
}
//* d * d
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>:: laplacian_up() const -> SparseMatrix {
    //h<1>
    return cod<D+1>() * d<D>();
}
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>:: laplacian() const -> SparseMatrix {
    if constexpr(D == 0) {
        return laplacian_up<D>();
    } else if(D == Dim) {
        return laplacian_down<D>();
    } else {
        return laplacian_down<D>() + laplacian_down<D>();
    }
}
//d * d that goes up down a dimension
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>:: weak_laplacian_down() const -> SparseMatrix {
    auto a = d<D-1>();
    return a * hi<D-1>().asDiagonal() * a.transpose();
}
//d * d that goes up a dimension
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>:: weak_laplacian_up() const -> SparseMatrix {
    auto a = d<D>();
    return a.transpose() * h<D+1>().asDiagonal() * a;
}
//d x
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::weak_poisson_down_rhs(const VecX& o) const -> VecX {
    return d<D>(o);
}
//d * x
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::weak_poisson_up_rhs(const VecX& o) const -> VecX {
    return d<D-1>().transpose() * h<D>(o);
}
//(d * d )^{-1} ( d x )
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::weak_poisson_down(const VecX& rhs) const -> VecX {
    auto L = weak_laplacian_down<D>();
    Eigen::ConjugateGradient<SparseMatrix, Eigen::Upper|Eigen::Lower> solver(L);
    VecX p = solver.solveWithGuess(rhs,VecX::Zero(rhs.rows()));
    return p;
}
//(d * d )^{-1} ( d * x )
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::weak_poisson_up(const VecX& rhs) const -> VecX {
    auto L = weak_laplacian_up<D>();
    Eigen::ConjugateGradient<SparseMatrix, Eigen::Upper|Eigen::Lower> solver(L);
    VecX p = solver.solveWithGuess(rhs,VecX::Zero(rhs.rows()));
    return p;
}

//Hodge decomposition components
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::exact_up(const VecX& o) const -> VecX {
    //d (u - HdHp) = 0
    //dHdHp = du
    //d^THdHp = du
    //Hp =(d^THd)^{-1} du


    //d<D> | hi<D>dt<D>   (h<D+1>)


    return hi<D+1>(weak_poisson_down<D+1>(weak_poisson_down_rhs<D>(o)));
}
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::coexact_down(const VecX& o) const -> VecX {
    //HdH (u - dp) = 0
    //HdHdp = HdHu
    //dHd^Tp = d^THu
    //p =(dHd^T)^{-1} dHu


    //(hi<D>)    dt<D-1>h<D> | d<D-1>
    return weak_poisson_up<D-1>(weak_poisson_up_rhs<D>(o));
}
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::exact(const VecX& o) const -> VecX {
    return d<D-1>(coexact_down<D>(o));
}
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::coexact(const VecX& o) const -> VecX {
    return cod<D+1>(exact_up<D>(o));
}
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::closed(const VecX& o) const -> VecX {
    return o - coexact<D>(o);
}
template <typename Derived, typename T, int EmbeddedDim, int Dim> template <int D>
auto DECMeshCore<Derived,T,EmbeddedDim,Dim>::coclosed(const VecX& o) const -> VecX {
    return o - exact<D>(o);
}
}}}
