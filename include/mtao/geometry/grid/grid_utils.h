#pragma once
#include <mtao/types.hpp>
#include <array>
#include <tuple>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace mtao::geometry::grid::utils {
    namespace internal {
        template <int N, int M, int Mask, typename coord_type, typename Func, bool Reverse = false, bool Parallel = false>
            struct masked_multi_looper {
                constexpr static bool Masked(int idx) {
                    return !bool(Mask & (1 << idx));
                }
                constexpr static int MyN = Reverse?M-N-1:N;
                static void run(const coord_type& bounds, coord_type& idx, const Func& f) {
                    if constexpr(Masked(N)) {
                        masked_multi_looper<N+1,M,Mask,coord_type,Func,Reverse>::run(bounds,idx,f);

                    } else if constexpr(false && M >= N+2 && !Masked(N+1)) {


                        constexpr static int NN = N+1;
                        constexpr static int MyNN = Reverse?M-NN-1:NN;
                        if constexpr(Parallel) {
#ifdef _OPENMP
#pragma omp parallel for
#endif
                            for(auto& i = idx[MyN] = 0; i < bounds[MyN]; ++i) {
                                for(auto& j = idx[MyNN] = 0; j < bounds[MyNN]; ++j) {
                                    masked_multi_looper<N+2,M,Mask,coord_type,Func,Reverse>::run(bounds,idx,f);
                                }
                            }
                        } else {
                            for(auto& i = idx[MyN] = 0; i < bounds[MyN]; ++i) {
                                for(auto& j = idx[MyNN] = 0; j < bounds[MyNN]; ++j) {
                                    masked_multi_looper<N+2,M,Mask,coord_type,Func,Reverse>::run(bounds,idx,f);
                                }
                            }
                        }
                        idx[MyNN] = 0;
                    } else {
                        if constexpr(Parallel) {
#ifdef _OPENMP
#pragma omp parallel for
#endif
                            for(auto&& i = idx[MyN] = 0; i < bounds[MyN]; ++i) {
                                masked_multi_looper<N+1,M,Mask,coord_type,Func,Reverse>::run(bounds,idx,f);
                            }
                        } else {
                            for(auto&& i = idx[MyN] = 0; i < bounds[MyN]; ++i) {
                                masked_multi_looper<N+1,M,Mask,coord_type,Func,Reverse>::run(bounds,idx,f);
                            }
                        }
                        idx[MyN] = 0;
                    }
                }
                static void run(const coord_type& begin, const coord_type& end, coord_type& idx, const Func& f) {
                    if constexpr(Masked(N)) {
                        masked_multi_looper<N+1,M,Mask,coord_type,Func,Reverse>::run(begin,end,idx,f);
                    } else if constexpr(false && M >= N+2 && !Masked(N+1)) {
                        constexpr static int NN = N+1;
                        constexpr static int MyNN = Reverse?M-NN-1:NN;
                        if constexpr(Parallel) {
#ifdef _OPENMP
#pragma omp parallel for
#endif
                            for(auto& i = idx[MyN] = begin[MyN]; i < end[MyN]; ++i) {
                                for(auto& j = idx[MyNN] = begin[MyNN]; j < end[MyNN]; ++j) {
                                    masked_multi_looper<N+2,M,Mask,coord_type,Func,Reverse>::run(begin,end,idx,f);
                                }
                            }
                        } else {
                            for(auto& i = idx[MyN] = begin[MyN]; i < end[MyN]; ++i) {
                                for(auto& j = idx[MyNN] = begin[MyNN]; j < end[MyNN]; ++j) {
                                    masked_multi_looper<N+2,M,Mask,coord_type,Func,Reverse>::run(begin,end,idx,f);
                                }
                            }
                        }
                        idx[MyNN] = begin[MyNN];
                    } else {
                        if constexpr(Parallel) {
#ifdef _OPENMP
#pragma omp parallel for
#endif
                            for(auto&& i = idx[MyN] = begin[MyN]; i < end[MyN]; ++i) {
                                masked_multi_looper<N+1,M,Mask,coord_type,Func,Reverse>::run(begin,end,idx,f);
                            }
                        } else {
                            for(auto&& i = idx[MyN] = begin[MyN]; i < end[MyN]; ++i) {
                                masked_multi_looper<N+1,M,Mask,coord_type,Func,Reverse>::run(begin,end,idx,f);
                            }
                        }
                    }
                    idx[MyN] = begin[MyN];
                }
            };

        template <int N, int Mask, typename coord_type, typename Func, bool Reverse, bool Parallel>
            struct masked_multi_looper<N,N, Mask,coord_type,Func,Reverse, Parallel> {
                static void run(const coord_type& bounds, coord_type& idx, const Func& f) {
                    f(idx);
                }
                static void run(const coord_type& begin, const coord_type& end, coord_type& idx, const Func& f) {
                    f(idx);
                }
            };
        template <int N, int M, typename coord_type, typename Func, bool Reverse = false>
            using multi_looper = masked_multi_looper<N,M,((1<<M) - 1),coord_type, Func, Reverse, false>;
#ifdef _OPENMP
        template <int N, int M, typename coord_type, typename Func, bool Reverse = false>
            using multi_looper_parallel = masked_multi_looper<N,M,((1<<M) - 1),coord_type, Func, Reverse, true>;
#else
        template <int N, int M, typename coord_type, typename Func, bool Reverse = false>
            using multi_looper_parallel = masked_multi_looper<N,M,((1<<M) - 1),coord_type, Func, Reverse, true>;
#endif
    }


    // Mask is an integer M such that every dimension d s.t (1 << d) & M == 0, the looper skips that dimension
    template <int Mask, typename coord_type, typename Func>
        void masked_multi_loop(const coord_type& index, const Func& f) {
            coord_type idx = {};
            internal::masked_multi_looper<0,std::tuple_size<coord_type>::value,Mask,coord_type,Func,false>::run(index,idx,f);
        }
    template <typename coord_type, typename Func>
        void multi_loop(const coord_type& index, const Func& f) {
            coord_type idx = {};
            internal::multi_looper<0,std::tuple_size<coord_type>::value,coord_type,Func,false>::run(index,idx,f);
        }
    template <typename coord_type, typename Func>
        void multi_loop_parallel(const coord_type& index, const Func& f) {
            coord_type idx = {};
            internal::multi_looper<0,std::tuple_size<coord_type>::value,coord_type,Func,false>::run(index,idx,f);
        }
    template <typename coord_type, typename Func>
        void right_multi_loop(const coord_type& index, const Func& f) {//Same above but does dimensions in reverse
            coord_type idx = {};
            std::fill(idx.begin(),idx.end(),0);
            internal::multi_looper<0,std::tuple_size<coord_type>::value,coord_type,Func,true>::run(index,idx,f);
        }

    template <typename coord_type, typename Func>
        void multi_loop(const coord_type& begin, const coord_type& end, const Func& f) {
            coord_type idx(begin);
            internal::multi_looper<0,std::tuple_size<coord_type>::value,coord_type,Func,false>::run(begin,end,idx,f);
        }
    template <typename coord_type, typename Func>
        void multi_loop_parallel(const coord_type& begin, const coord_type& end, const Func& f) {
            coord_type idx(begin);
            internal::multi_looper<0,std::tuple_size<coord_type>::value,coord_type,Func,false>::run(begin,end,idx,f);
        }
    template <typename coord_type, typename Func>
        void right_multi_loop(const coord_type& begin, const coord_type& end, const Func& f) {//Same above but does dimensions in reverse
            coord_type idx(begin);
            internal::multi_looper<0,std::tuple_size<coord_type>::value,coord_type,Func,true>::run(begin,end,idx,f);
        }
}





namespace mtao {
    namespace interp_internal {
        template <typename T, typename U>
            T lerp(const T& a, const T& b, U alpha) {//alpha = 0 -> a, alpha = 1 -> b
                return (1-alpha) * a + alpha * b;
            }
        template <typename T, typename U>
            T bilerp(const T& a, const T& b, const T& c, const T& d, U alpha, U beta) {
                return lerp(lerp(a,b,alpha), lerp(c,d,alpha),beta);// 0,0 => a, 0,1 => b, 1,0 => c, 1,1 => d
            }
        template <typename T, typename U>
            T trilerp(const T& a, const T& b, const T& c, const T& d, const T& e, const T& f, const T& g, const T& h, U alpha, U beta, U gamma) {
                return lerp(
                        bilerp(a,b,c,d,alpha,beta)// 0,0,0 => a, 0,1,0 => b, 1,0,0 => c, 1,1,0 => d
                        ,
                        bilerp(e,f,g,h,alpha,beta)// 0,0,1 => e, 0,1,1 => f, 1,0,1 => g, 1,1,1 => h
                        ,
                        gamma);
            }

        template <typename GridType, typename U>
            typename GridType::Scalar bilerp(const GridType& g, int i, int j, U alpha, U beta) {
                return bilerp(g(i,j),g(i+1,j  ),g(i,j+1  ),g(i+1,j+1),alpha,beta);
            }
        template <typename GridType, typename U>
            typename GridType::Scalar trilerp(const GridType& g, int i, int j, int k, U alpha, U beta, U gamma) {
                return trilerp(
                        g(i,j,k  ),g(i+1,j  ,k  ),g(i,j+1  ,k  ),g(i+1,j+1,k  )
                        ,
                        g(i,j,k+1),g(i+1,j  ,k+1),g(i,j+1  ,k+1),g(i+1,j+1,k+1)
                        ,alpha,beta,gamma);
            }
    }

    template <typename T>
        void barycentric(T a, int ni, int* i, T* di ) {
            constexpr static T ome = 1 - std::numeric_limits<T>::epsilon();
            a = std::min<T>(std::max<T>(a,0),ni);
            T v = a;
            T v2 = std::floor(v);
            *di = v-v2;
            *i = int(v2);
            if(*i < 0) {
                *di = 0;
                *i = 0;
            } else if(*i >= ni) {
                *i = ni-1;
                *di = ome;
            }
        }

    template <typename VecType, typename ShapeType,typename DiffType, int D = std::tuple_size<ShapeType>::value>
        void barycentric(const VecType& v, const ShapeType& shape, ShapeType& i, DiffType& di) {
            static_assert(std::tuple_size<DiffType>::value == D,"");
            for(size_t idx = 0; idx < D; ++idx) {
                barycentric(v(idx),shape[idx]-1,&i[idx],&di[idx]);
            }
        }
    template <typename T, typename GridType>
        typename GridType::Scalar lerp(const GridType& g, const std::array<int,GridType::D>& i, const std::array<T,GridType::D>& di);

    template <typename T, typename GridType>
        typename GridType::Scalar lerp(const GridType& g, const std::array<int,2>& i, const std::array<T,2>& di) {
            static_assert(GridType::D == 2,"");
            return mtao::interp_internal::bilerp(g,i[0],i[1],di[0],di[1]);
        }
    template <typename T, typename GridType>
        typename GridType::Scalar lerp(const GridType& g, const std::array<int,3>& i, const std::array<T,3>& di) {
            static_assert(GridType::D == 3,"");
            return mtao::interp_internal::trilerp(g,i[0],i[1],i[2],di[0],di[1],di[2]);
        }
    template <typename VecType, typename GridType>
        typename GridType::Scalar lerp(const GridType& g, const VecType& v) {
            constexpr int Dim = GridType::D;//mtao::Grid assumption
            //int i,j;
            //float di,dj;
            std::array<int,Dim> i;
            std::array<typename VecType::Scalar,Dim> di;
            barycentric(v,g.shape(),i,di);
            return lerp(g,i,di);
        }
    template <typename VecType, typename GridType>
        typename GridType::Scalar bilerp(const GridType& g, const VecType& v) {
            return lerp(g,v);
        }
}




