#ifndef STATIC_ITERATION_H
#define STATIC_ITERATION_H
namespace mtao {

    template <typename T, typename FuncType, int D, int I = 0, typename = std::enable_if<(I < D), std::nullptr_t>::value>
    void _run_static_times(const T& type) {
            FuncType(T.template get<D>());
            if(D > I) {
                _static_run_times<T,FuncType,D,I+1>(type,func);
            }
    }

    template <int D, typename T, typename FuncType>
        void run_static_times(const T& type, const FuncType& func) {
            _static_run_times<T,FuncType,D>(type,func);
        }

}
#endif//STATIC_ITERATION_H
