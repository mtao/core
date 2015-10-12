#ifndef STATIC_ITERATION_H
#define STATIC_ITERATION_H
#include "util.h"
#include "compat.h"
namespace mtao {

    namespace internal {
        template <typename T, typename FuncType, int D, int I = 0>
            struct static_iterator: public static_iterator<T,FuncType,D,I+1> {
                //No return types
                static void run(const FuncType& func, const T& data) {
                    func(data.template get<I>());
                    static_iterator<T,FuncType,D,I+1>::run(func,data);
                }
                static void run(const FuncType& func,T& data ) {
                    func(data.template get<I>());
                    static_iterator<T,FuncType,D,I+1>::run(func,data);
                }
                //With return types
                template <typename RetObj>
                    static void run(const FuncType& func, RetObj& ret,const T& data ) {
                        ret[I] = func(data.template get<I>());
                        static_iterator<T,FuncType,D,I+1>::run(func,ret,data);
                    }
                template <typename RetObj>
                    static void run(const FuncType& func, RetObj& ret,T& data) {
                        ret[I] = func(data.template get<I>());
                        static_iterator<T,FuncType,D,I+1>::run(func,ret,data);
                    }
            };

        template <typename T, typename FuncType, int D>
            struct static_iterator<T,FuncType,D,D> {
                static void run(const FuncType& func,const T& data) {
                }
                template <typename RetObj>
                    static void run(const FuncType& func, RetObj&,const T& data) {
                    }
            };
    }

    template <int D, typename T, typename FuncType>
        void static_loop( const FuncType& func,T& data) {
            internal::static_iterator<T,FuncType,D,0>::run(func,data);
        }
    template <int D, typename T, typename FuncType, typename RetObj>
        void static_loop(const FuncType& func, RetObj& ret,T& data ) {
            internal::static_iterator<T,FuncType,D,0>::run(func,ret,data);
        }

    template <int D, typename T, typename FuncType>
        void const_static_loop(const FuncType& func,const T& data) {
            internal::static_iterator<T,FuncType,D,0>::run(func,data);
        }
    template <int D, typename T, typename FuncType, typename RetObj>
        void const_static_loop(const FuncType& func, RetObj& ret,const T& data) {
            internal::static_iterator<T,FuncType,D,0>::run(func,ret,data);
        }

}
#endif//STATIC_ITERATION_H
