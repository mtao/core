#ifndef TYPE_UTILS_H
#define TYPE_UTILS_H

namespace mtao {

//https://stackoverflow.com/questions/11251376/how-can-i-check-if-a-type-is-an-instantiation-of-a-given-class-template#comment14786989_11251408
template < template <typename...> class Template, typename T >
struct is_specialization_of : std::false_type {};

template < template <typename...> class Template, typename... Args >
struct is_specialization_of< Template, Template<Args...> > : std::true_type {};

template < template <typename...> class Template, typename T >
constexpr bool is_specialization_of_v = is_specialization_of<Template,T>::value;
}
#endif//TYPE_UTILS_H
