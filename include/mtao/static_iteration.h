#ifndef STATIC_ITERATION_H
#define STATIC_ITERATION_H
#include "util.h"
#include "compat.h"
namespace mtao {

    namespace internal {
    template <int... M, typename FuncType, typename... Args>
        void static_loop(std::integer_sequence<M,...>, const FuncType& func,Args&&... data) {
            (func(M,std::forward<Args>(data)...),...);
        }
    template <int... M, typename FuncType, typename RetObj, typename... Args>
        void static_loop(std::integer_sequence<M,...>, const FuncType& func, RetObj& r,Args&&... data) {
            (r[M].operator=(func(M,std::forward<Args>(data)...)),...);
        }
    }

    template <int D, typename FuncType, typename... Args>
        void static_loop( const FuncType& func,Args&&... data) {
            internal::static_loop<(std::make_integer_sequence<int,D>(),func,std::forward<Args>(data)...);
        }
    template <int D,  typename FuncType, typename RetObj, typename... Args>
        void static_loop_ret(const FuncType& func, RetObj& ret,Args&&... data ) {
            internal::static_loop<(std::make_integer_sequence<int,D>(),fun,ret,std::forward<Args>(data)...);
        }


}
#endif//STATIC_ITERATION_H
