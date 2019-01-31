#ifndef TYPE_UTILS_H
#define TYPE_UTILS_H
#include <cxxabi.h>
#include <type_traits>
#include <string>

namespace mtao { namespace types {

    //https://stackoverflow.com/questions/11251376/how-can-i-check-if-a-type-is-an-instantiation-of-a-given-class-template#comment14786989_11251408
    template < template <typename...> class Template, typename T >
        struct is_specialization_of : std::false_type {};

    template < template <typename...> class Template, typename... Args >
        struct is_specialization_of< Template, Template<Args...> > : std::true_type {};

    template < template <typename...> class Template, typename T >
        constexpr bool is_specialization_of_v = is_specialization_of<Template,T>::value;

    template <typename T>
        std::string getTypeName() {

            const char* const name = typeid(T).name();
            int status = 0;
            using abi::__cxa_demangle;
            char* const readable_name = __cxa_demangle(name, 0, 0, &status);
            std::string name_str(status == 0 ? readable_name : name);
            free(readable_name);
            if constexpr(std::is_reference_v<T>) {
                name_str = name_str + "&";
            }
            return name_str;
        }

    template <typename T>
        std::string getTypeName(T&& t) {
            return getTypeName<T>();
        }

    template<typename T, class = void>
        struct has_tuple_size: public std::false_type {};

    template<class T>
        struct has_tuple_size< T, std::enable_if_t<std::tuple_size<T>() == std::tuple_size<T>()>>: public std::true_type{};

    template <typename T>
        constexpr size_t container_size(const T& container) {
            if constexpr(has_tuple_size<T>()) {
                return std::tuple_size<T>();
            } else {
                return container.size();
            }
        }

template <typename T >
constexpr int check_size(T&& t) {
    if(has_tuple_size<T>()) {
        return 2;
    } else {
        return -1;
    } 
    return -1;

}

    template <typename T, int N>
        constexpr size_t container_size(const T (&V)[N]) {
            return N;
        }
    template <typename T>
        struct is_initializer_list: public std::false_type {
        };
    template <typename T>
        struct is_initializer_list<std::initializer_list<T>>: public std::true_type {
        };
    template <typename T>
        constexpr static bool is_initializer_list_v = is_initializer_list<T>::value;

    template <typename T>
        using remove_cvref_t = std::remove_reference_t<std::remove_cv_t<T>>;
}}
#endif//TYPE_UTILS_H
