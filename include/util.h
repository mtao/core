#ifndef UTIL_H
#define UTIL_H
namespace mtao {
    template <typename T>
        T clamp(const T& value, const T& a, const T& b) {
            if(value < a) {
                return a;
            } else if(value > b) {
                return b;
            } else {
                return value;
            }
        }
    namespace internal {
        constexpr int recursive_pow(int a, int b) {
            if(b == 0) {
                return 1;
            }
            return a * recursive_pow(a,b-1);
        }
    }
    template <int A, int B, typename = std::enable_if<(B>=0),std::nullptr_t>::value>
        struct pow {
            constexpr static int value = recursive_pow(A,B);
        };

    /*
    template <typename ContainerType>
        constexpr size_t size(const ContainerType& c) {
            return c.size();
        }
        */
}
#endif//UTIL_H
