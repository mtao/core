#ifndef TYPE_UTILS_H
#define TYPE_UTILS_H
#include <cxxabi.h>

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
  const std::string name_str(status == 0 ? readable_name : name);
  free(readable_name);
  return name_str;
}

template <typename T>
std::string getTypeName(const T& t) {
    return getTypeName<T>();
}
}}
#endif//TYPE_UTILS_H
