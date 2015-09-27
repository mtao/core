#ifndef GRID_UTILS_H
#define GRID_UTILS_H
#include <Eigen/Dense>

namespace mtao {

    namespace internal {
        template <typename T>
            struct allocator_selector {
                using type = std::allocator<T>;
            };

        template <typename T, int R, int C>
            struct allocator_selector<Eigen::Matrix<T,R,C>> {
                using type = Eigen::aligned_allocator<Eigen::Matrix<T,R,C>>;
            };
    }

    template <typename T>
        using allocator = typename internal::allocator_selector<T>::type;


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
        };
    template <int N, typename index_type, typename Func, bool Reverse>
        struct multi_looper<N,N,index_type,Func,Reverse> {
                static void run(const index_type& bounds, index_type& idx, const Func& f) {
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
        void right_multi_loop(const index_type& index, const Func& f) {
            index_type idx;
            std::fill(idx.begin(),idx.end(),0);
            internal::multi_looper<0,mtao::compat::tuple_size<index_type>::value,index_type,Func,true>::run(index,idx,f);
        }
}

#endif//GRID_UTILS_H
