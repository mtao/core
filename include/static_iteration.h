#ifndef STATIC_ITERATION_H
#define STATIC_ITERATION_H
#include "util.h"
#include "compat.h"
namespace mtao {

    namespace internal {
        template <typename T, typename FuncType, int D, int I = 0>
            struct static_iterator: public static_iterator<T,FuncType,D,I+1> {
                static void run(const T& type, const FuncType& func) {
                    func(type.template get<I>());
                    static_iterator<T,FuncType,D,I+1>::run(type,func);
                }
                template <typename RetObj>
                    static void run(const T& type, const FuncType& func, RetObj& ret) {
                        ret[I] = func(type.template get<I>());
                        static_iterator<T,FuncType,D,I+1>::run(type,func,ret);
                    }
                static void run(T& type, const FuncType& func) {
                    func(type.template get<I>());
                    static_iterator<T,FuncType,D,I+1>::run(type,func);
                }
                template <typename RetObj>
                    static void run(T& type, const FuncType& func, RetObj& ret) {
                        ret[I] = func(type.template get<I>());
                        static_iterator<T,FuncType,D,I+1>::run(type,func,ret);
                    }
            };

        template <typename T, typename FuncType, int D>
            struct static_iterator<T,FuncType,D,D> {
                static void run(const T& type, const FuncType& func) {
                }
                template <typename RetObj>
                    static void run(const T& type, const FuncType& func, RetObj&) {
                    }
            };
    }

    template <int D, typename T, typename FuncType>
        void static_loop(T& type, const FuncType& func) {
            internal::static_iterator<T,FuncType,D,0>::run(type,func);
        }
    template <int D, typename T, typename FuncType, typename RetObj>
        void static_loop(T& type, const FuncType& func, RetObj& ret) {
            internal::static_iterator<T,FuncType,D,0>::run(type,func,ret);
        }

    template <int D, typename T, typename FuncType>
        void const_static_loop(const T& type, const FuncType& func) {
            internal::static_iterator<T,FuncType,D,0>::run(type,func);
        }
    template <int D, typename T, typename FuncType, typename RetObj>
        void const_static_loop(const T& type, const FuncType& func, RetObj& ret) {
            internal::static_iterator<T,FuncType,D,0>::run(type,func,ret);
        }

}
#endif//STATIC_ITERATION_H
