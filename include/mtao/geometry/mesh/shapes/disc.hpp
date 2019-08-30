#pragma once
#include <mtao/eigen/stack.h>

namespace mtao::geometry::mesh::shapes {
    namespace internal {
        template <bool CenterPoint=false>
        mtao::ColVecs3i disc_faces(int N) {
            if constexpr(CenterPoint) {
                mtao::ColVecs3i CF(3,N);
                for(int i = 0; i < N; ++i) {
                    CF.col(i) << 0,(i)%N+1,(i+1)%N+1;
                }
                return CF;
            } else {
                mtao::ColVecs3i CF(3,N-2);
                for(int i = 1; i < N-1; ++i) {
                    CF.col(i-1) << 0,i+1,i;
                }
                return CF;
            }
        }
        template <bool CenterPoint=false, typename T = double>
            mtao::ColVectors<T,3> disc_points(const mtao::Vector<T,3>& U, const mtao::Vector<T,3>& V, int N) {
                using VecX = mtao::VectorX<T>;
                VecX theta = VecX::LinSpaced(N+1,0,2*M_PI).head(N);

                mtao::ColVectors<T,2> CS = mtao::eigen::hstack(theta.array().cos(),theta.array().sin()).transpose();
                mtao::ColVectors<T,3> R = mtao::eigen::hstack(U,V) * CS;
                if constexpr(CenterPoint) {
                    R = mtao::eigen::hstack(mtao::Vector<T,3>::Zero(),R);
                }
                return R;
            }
    }

        template <bool CenterPoint=false,typename T=double>
        std::tuple<mtao::ColVectors<T,3>,mtao::ColVecs3i> disc(const mtao::Vector<T,3>& U, const mtao::Vector<T,3>& V, int N=5) {
            return {internal::disc_points<CenterPoint>(U,V,N),internal::disc_faces<CenterPoint>(N)};
        }


}
