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

    /*
    template <typename ContainerType>
        constexpr size_t size(const ContainerType& c) {
            return c.size();
        }
        */
}
#endif//UTIL_H
