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

}
#endif//UTIL_H
