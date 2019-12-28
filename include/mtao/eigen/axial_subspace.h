#ifndef EIGEN_AXIAL_SUBSPACE_H
#define EIGEN_AXIAL_SUBSPACE_H

#include "mtao/types.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>
namespace mtao { namespace eigen {
    template <typename T>
    class AxialSubspace {
        using Triplet = Eigen::Triplet<T>;
        std::vector<Triplet> reduced_triplets;
        using PermutationType = Eigen::PermutationMatrix<Eigen::Dynamic,Eigen::Dynamic>;
        using SparseMatrix =Eigen::SparseMatrix<T>;
        using VecX = mtao::VectorX<T>;
        PermutationType permuter;//full -> reduced
        PermutationType invpermuter;
        VecX filter;
        SparseMatrix ghost_values;
        int full_size;
        int active_size;

        AxialSubspace(const PermutationType& pt, int fs, int as, std::vector<Triplet>&& trp = std::vector<Triplet>()): reduced_triplets(std::move(trp)), permuter(pt), invpermuter(pt.transpose()), full_size(fs),active_size(as) {
            reduce_in_place(reduced_triplets);
            filter = VecX::Zero(full_size);
            for(int i = 0; i < active_size; ++i) {
                filter(invpermuter.indices()(i)) = 1;
            }

            ghost_values = SparseMatrix(full_size,full_size);

        }




        //Boundary stuff
        void append(std::vector<Triplet>&& triplets) {
            reduce_in_place(triplets);
            reduced_triplets.reserve(reduced_triplets.size()+triplets.size());
            reduced_triplets.insert(reduced_triplets.end(),triplets.begin(),triplets.end());
        }
        void append(const SparseMatrix& A) {
            append(m2t(filtrate(A)));
        }
        int reduce(int i) const {
            return permuter.indices()(i);
        }

        void reduce_in_place(std::vector<Triplet>& triplets) const {
            triplets.erase(std::remove_if(triplets.begin(),triplets.end(),[](auto&& t) {
                        return std::abs(t.value()) < 1e-5;
                        }
                        ),triplets.end());
            permute_in_place(triplets,permuter);
        }
        static void permute_in_place(std::vector<Triplet>& triplets, const PermutationType& p) {
            std::transform(triplets.begin(),triplets.end(),triplets.begin(),
                    [&](const Triplet& t) {

                    return Triplet(p.indices()(t.row()),p.indices()(t.col()),t.value());
                    });
        }

        SparseMatrix filtrate(const SparseMatrix& A) const {
            return filter.asDiagonal() * A * filter.asDiagonal();
        }

        VecX filtrate(const VecX& v) const {
            VecX ret = VecX::Zero(v.rows());
            auto&& block = ret.topRows(full_size);
            block = v.topRows(full_size).cwiseProduct(filter);
            return ret;
        }

        std::vector<Triplet> reduce(const std::vector<Triplet>& triplets) const {
            std::vector<Triplet> ret(triplets);
            reduce_in_place(ret);
            return ret;

        }
        //matrix to triplets
        std::vector<Triplet> m2t(const SparseMatrix& A) const {
            std::vector<Triplet> ret(A.nonZeros());
            size_t counter = 0;
            for(int i = 0; i < A.rows(); ++i) {
                for(Eigen::SparseMatrix<float>::InnerIterator it(A,i); it; ++it) {
                    ret[counter++] = Triplet(it.row(),it.col(),it.value());
                }
            }
            return ret;
        }

        VecX expand(const VecX& o) const {
            assert(o.rows() == active_size);
            VecX full_unpermuted = VecX::Zero(permuter.rows());
            full_unpermuted.topRows(active_size) = o;
            full_unpermuted = invpermuter * full_unpermuted;
            /*
               for(int i = 0; i < full_unpermuted.rows(); ++i) {
               auto&& v = full_unpermuted(i);
               if(v != 0) {
               v /= std::abs(v);
               }
               }
               */
            auto&& fut = full_unpermuted.topRows(full_size);
            fut += ghost_values * fut;
            return full_unpermuted;

        }
        VecX reduce(const VecX& o) const {
            assert(o.rows() >= full_size);
            VecX permuted = (permuter * o.topRows(full_size)).topRows(active_size);
            return permuted;

        }
        SparseMatrix reduced_operator() const {
            SparseMatrix ret(active_size,active_size);
            ret.setFromTriplets(reduced_triplets.begin(),reduced_triplets.end());
            return ret;
        }
        SparseMatrix full_operator() const {

            PermutationType pi = permuter.transpose();
            std::vector<Triplet> trp = reduced_triplets;
            permute_in_place(trp,pi);

            SparseMatrix ret(full_size,full_size);
            ret.setFromTriplets(trp.begin(),trp.end());
            return ret;
        }
    };
}}
#endif//EIGEN_AXIAL_SUBSPACE_H
