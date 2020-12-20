
#ifndef TYPE_UTILS_HPP
#define TYPE_UTILS_HPP
#if defined(_MSC_VER)
#include <Windows.h>
#include <tchar.h>
#else
#include <cxxabi.h>
#endif
#include <string>
#include <tuple>
#include <type_traits>

namespace mtao::types {
[[deprecated("Should replace this with std::nullopt_t")]] class empty {};

// https://stackoverflow.com/questions/11251376/how-can-i-check-if-a-type-is-an-instantiation-of-a-given-class-template#comment14786989_11251408
template <template <typename...> class Template, typename T>
struct is_specialization_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template, Template<Args...>> : std::true_type {};

template <template <typename...> class Template, typename T>
constexpr bool is_specialization_of_v =
    is_specialization_of<Template, T>::value;

namespace internal {
template <class T, std::size_t = sizeof(T)>
std::true_type is_derived_specialization_impl(T*);

std::false_type is_derived_specialization_impl(...);
}  // namespace internal

template <class T>
using is_derived_specialization =
    decltype(internal::is_derived_specialization_impl(std::declval<T*>()));

template <class T>
bool is_derived_specialization_v = is_derived_specialization<T>::value;

template <typename T>
std::string getTypeName() {
    const char* const name = typeid(T).name();
    int status = 0;
    // TODO: Actually demangle in msc
#if defined(_MSC_VER)
    const char* const readable_name = name;
#else
    using abi::__cxa_demangle;
    char* const readable_name = __cxa_demangle(name, 0, 0, &status);
#endif
    std::string name_str(status == 0 ? readable_name : name);
#if !defined(_MSC_VER)
    free(readable_name);
#endif
    if constexpr (std::is_reference_v<T>) {
        name_str = name_str + "&";
    }
    return name_str;
}

template <typename T>
std::string getTypeName(T&& t) {
    return getTypeName<T>();
}

template <typename T, class = void>
struct has_tuple_size : public std::false_type {};

template <class T>
struct has_tuple_size<
    T, std::enable_if_t<std::tuple_size<T>() == std::tuple_size<T>()>>
    : public std::true_type {};

template <typename T>
constexpr size_t container_size(const T& container) {
    if constexpr (has_tuple_size<T>()) {
        return std::tuple_size<T>();
    } else {
        return container.size();
    }
}

template <typename T>
constexpr int check_size(T&& t) {
    if (has_tuple_size<T>()) {
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
struct is_initializer_list : public std::false_type {};
template <typename T>
struct is_initializer_list<std::initializer_list<T>> : public std::true_type {};
template <typename T>
constexpr static bool is_initializer_list_v = is_initializer_list<T>::value;

template <typename T>
using remove_cvref_t [[deprecated]] =
    std::remove_cv_t<std::remove_reference_t<T>>;

//===============================================
// Copied from https://devblogs.microsoft.com/oldnewthing/20200629-00/?p=103910
namespace internal {
template <typename T, typename Tuple>
struct tuple_element_index_helper;
template <typename T>
struct tuple_element_index_helper<T, std::tuple<>> {
    static constexpr std::size_t value = 0;
};
template <typename T, typename... Rest>
struct tuple_element_index_helper<T, std::tuple<T, Rest...>> {
    static constexpr std::size_t value = 0;
    using RestTuple = std::tuple<Rest...>;
    static_assert(tuple_element_index_helper<T, RestTuple>::value ==
                      std::tuple_size_v<RestTuple>,
                  "type appears more than once in tuple");
};
template <typename T, typename First, typename... Rest>
struct tuple_element_index_helper<T, std::tuple<First, Rest...>> {
    using RestTuple = std::tuple<Rest...>;
    static constexpr std::size_t value =
        1 + tuple_element_index_helper<T, RestTuple>::value;
};

}  // namespace internal
template <typename T, typename Tuple>
struct tuple_element_index {
    static constexpr std::size_t value =
        internal::tuple_element_index_helper<T, Tuple>::value;
    static_assert(value < std::tuple_size_v<Tuple>,
                  "type does not appear in tuple");
};
template <typename T, typename Tuple>
inline constexpr std::size_t tuple_element_index_v =
    tuple_element_index<T, Tuple>::value;
//===============================================

}  // namespace mtao::types
#endif  // TYPE_UTILS_H
