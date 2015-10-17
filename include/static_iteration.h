#ifndef STATIC_ITERATION_H
#define STATIC_ITERATION_H
#include "util.h"
#include "compat.h"
namespace mtao {

    namespace internal {
        template <typename FuncType, int D, int I,typename... Args >
            struct static_iterator: public static_iterator<FuncType,D,I+1,Args...> {
                template <int J, typename Cont>
                    static const typename Cont::value_type& getN(const Cont& c) {
                        return c.template get<J>();
                    }
                template <int J, typename Cont>
                    static typename Cont::value_type& getN(Cont& c) {
                        return c.template get<J>();
                    }

                //No return types
                static void run(const FuncType& func, Args&... data) {
                    func(I,getN<I>(data)...);

                    static_iterator<FuncType,D,I+1,Args...>::run(func,std::forward<Args&>(data)...);
                }
                //With return types
                template <typename RetObj>
                    static void run(const FuncType& func, RetObj& ret,Args&... data ) {
                        ret[I] = func(I,getN<I>(data)...);
                        static_iterator<FuncType,D,I+1,Args...>::run(func,ret,std::forward<Args&>(data)...);
                    }
            };

        template <typename FuncType, int D, typename... Args>
            struct static_iterator<FuncType,D,D,Args...> {
                static void run(const FuncType& func,Args&... data) {
                }
                template <typename RetObj>
                    static void run(const FuncType& func, RetObj&,Args&... data) {
                    }
            };
    }

    template <int D, typename FuncType, typename... Args>
        void static_loop( const FuncType& func,Args&... data) {
            internal::static_iterator<FuncType,D,0,Args...>::run(func,std::forward<Args&>(data)...);
        }
    template <int D,  typename FuncType, typename RetObj, typename... Args>
        void static_loop_ret(const FuncType& func, RetObj& ret,Args&... data ) {
            internal::static_iterator<FuncType,D,0,Args...>::run(func,ret,std::forward<Args&>(data)...);
        }

    template <int D, typename FuncType, typename... Args>
        void const_static_loop(const FuncType& func,Args&... data) {
            internal::static_iterator<FuncType,D,0,Args...>::run(func,std::forward<Args&>(data)...);
        }
    template <int D, typename FuncType, typename RetObj, typename... Args>
        void const_static_loop_ret(const FuncType& func, RetObj& ret,Args&... data) {
            internal::static_iterator<FuncType,D,0,Args...>::run(func,ret,std::forward<Args&>(data)...);
        }

}
#endif//STATIC_ITERATION_H
