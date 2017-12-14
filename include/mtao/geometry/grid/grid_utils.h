#ifndef GRID_UTILS_H
#define GRID_UTILS_H

#include "types.h"
#include "compat.h"

namespace mtao {

    namespace internal {
        template <typename ArrayType>
        ArrayType zero_array() {
            ArrayType a;
            for(auto&& v: a) {
                v = typename ArrayType::value_type(0);
            }
            return a;
        }
        template <typename T>
            struct zero {
                constexpr static T s_value = 0;
                static T value() {
                    return s_value;
                }
            };
        template <typename T, int R, int C>
            struct zero<Eigen::Matrix<T,R,C>> {
                //static constexpr Eigen::Matrix<T,R,C> value = Eigen::Matrix<T,R,C>::Zero();
                static Eigen::Matrix<T,R,C> value() {
                    return  Eigen::Matrix<T,R,C>::Zero();
                }
            };
        template <typename T, int R, int C>
            struct zero<Eigen::Matrix<Eigen::Matrix<T,R,C>,R,C>> {
                //static constexpr Eigen::Matrix<T,R,C> value = Eigen::Matrix<T,R,C>::Zero();
                static Eigen::Matrix<Eigen::Matrix<T,R,C>,R,C> value() {
                    return  Eigen::Matrix<Eigen::Matrix<T,R,C>,R,C>();
                }
            };
            template <typename T>
            T zero_value() {
                return zero<T>::value();
            }
    }
    template <int D, typename index_type = mtao::compat::array<int,D> >
        size_t grid_index(const index_type& shape, const index_type& a) {
                        size_t idx = 0;
                        for(int i = D-1; i >=0; --i) {
                            assert(int(a[i]) < shape[i]);
                            idx = a[i] + shape[i] * idx;
                        }
                        return idx;
        }
    namespace internal {
        template <int N, int M, typename index_type, typename Func, bool Reverse = false>
            struct multi_looper {
                constexpr static int MyN = Reverse?M-N-1:N;
                static void run(const index_type& bounds, index_type& idx, const Func& f) {
                    for(auto&& i = idx[MyN] = 0; i < bounds[MyN]; ++i) {
                        multi_looper<N+1,M,index_type,Func,Reverse>::run(bounds,idx,f);
                    }
                    idx[MyN] = 0;
                }
                static void run(const index_type& begin, const index_type& end, index_type& idx, const Func& f) {
                    for(auto&& i = idx[MyN] = begin[MyN]; i < end[MyN]; ++i) {
                        multi_looper<N+1,M,index_type,Func,Reverse>::run(begin,end,idx,f);
                    }
                    idx[MyN] = begin[MyN];
                }
            };
        template <int N, typename index_type, typename Func, bool Reverse>
            struct multi_looper<N,N,index_type,Func,Reverse> {
                static void run(const index_type& bounds, index_type& idx, const Func& f) {
                    f(idx);
                }
                static void run(const index_type& begin, const index_type& end, index_type& idx, const Func& f) {
                    f(idx);
                }
            };
    }
    template <typename index_type, typename Func>
        void multi_loop(const index_type& index, const Func& f) {
            index_type idx;
            std::fill(idx.begin(),idx.end(),0);
            internal::multi_looper<0,mtao::compat::tuple_size<index_type>::value,index_type,Func,false>::run(index,idx,f);
        }
    template <typename index_type, typename Func>
        void right_multi_loop(const index_type& index, const Func& f) {//Same above but does dimensions in reverse
            index_type idx;
            std::fill(idx.begin(),idx.end(),0);
            internal::multi_looper<0,mtao::compat::tuple_size<index_type>::value,index_type,Func,true>::run(index,idx,f);
        }

    template <typename index_type, typename Func>
        void multi_loop(const index_type& begin, const index_type& end, const Func& f) {
            index_type idx(begin);
            internal::multi_looper<0,mtao::compat::tuple_size<index_type>::value,index_type,Func,false>::run(begin,end,idx,f);
        }
    template <typename index_type, typename Func>
        void right_multi_loop(const index_type& begin, const index_type& end, const Func& f) {//Same above but does dimensions in reverse
            index_type idx(begin);
            internal::multi_looper<0,mtao::compat::tuple_size<index_type>::value,index_type,Func,true>::run(begin,end,idx,f);
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
            a = std::min<T>(std::max<T>(a,0),ome);
            T v = a * (ni);
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

    template <typename VecType, typename ShapeType,typename DiffType, int D = mtao::compat::tuple_size<ShapeType>::value>
        void barycentric(const VecType& v, const ShapeType& shape, ShapeType& i, DiffType& di) {
            static_assert(mtao::compat::tuple_size<DiffType>::value == D,"");
            for(size_t idx = 0; idx < D; ++idx) {
                barycentric(v(idx),shape[idx]-1,&i[idx],&di[idx]);
            }
        }
    template <typename T, typename GridType>
        typename GridType::Scalar lerp(const GridType& g, const mtao::compat::array<int,GridType::D>& i, const mtao::compat::array<T,GridType::D>& di);

    template <typename T, typename GridType>
        typename GridType::Scalar lerp(const GridType& g, const mtao::compat::array<int,2>& i, const mtao::compat::array<T,2>& di) {
            static_assert(GridType::D == 2,"");
            return mtao::interp_internal::bilerp(g,i[0],i[1],di[0],di[1]);
        }
    template <typename T, typename GridType>
        typename GridType::Scalar lerp(const GridType& g, const mtao::compat::array<int,3>& i, const mtao::compat::array<T,3>& di) {
            static_assert(GridType::D == 3,"");
            return mtao::interp_internal::trilerp(g,i[0],i[1],i[2],di[0],di[1],di[2]);
        }
    template <typename VecType, typename GridType>
        typename GridType::Scalar lerp(const GridType& g, const VecType& v) {
            constexpr int Dim = GridType::D;//mtao::Grid assumption
            //int i,j;
            //float di,dj;
            mtao::compat::array<int,Dim> i;
            mtao::compat::array<typename VecType::Scalar,Dim> di;
            barycentric(v,g.shape(),i,di);
            return lerp(g,i,di);
        }
    template <typename VecType, typename GridType>
        typename GridType::Scalar bilerp(const GridType& g, const VecType& v) {
            return lerp(g,v);
        }
}





#endif//GRID_UTILS_H
