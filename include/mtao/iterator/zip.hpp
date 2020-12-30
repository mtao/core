#pragma once

#include <iostream>
#if defined(MTAO_USE_RANGESv3)
#include <range/v3/view/zip.hpp>

#endif
#include "mtao/type_utils.h"
#include "mtao/types.hpp"
#include "mtao/iterator/shell.hpp"

namespace mtao::iterator {
#if defined(MTAO_USE_RANGESv3)
template<typename... Types>
auto zip(Types &&... t) {
    return ranges::views::zip(std::forward<Types>(t)...);
    // return detail::zip_container<Types...>(std::forward<Types>(t)...);
}
#else
namespace detail {

    template<typename T, bool Expr = mtao::types::is_initializer_list_v<mtao::types::remove_cvref_t<T>>>
    struct filter_initializer_list {
        using value_type = std::vector<typename T::value_type>;
    };
    template<typename T>
    struct filter_initializer_list<T, false> {
        using value_type = T;
    };
    template<typename T>
    using filter_initializer_list_t =
      typename filter_initializer_list<T>::value_type;

    template<typename... IteratorTypes>
    struct zip_iterator {
      public:
        using tuple_type = std::tuple<IteratorTypes...>;

        zip_iterator(IteratorTypes &&... its)
          : m_its{ std::forward<IteratorTypes>(its)... } {}

        bool operator==(const zip_iterator &o) const { return equal_to(_is(), o); }
        bool operator!=(const zip_iterator &o) const { return !equal_to(_is(), o); }
        bool operator<(const zip_iterator &o) const { return less_than(_is(), o); }

        zip_iterator &operator++() { return increment(_is()); }

        auto operator*() { return dereference(_is()); }

      private:
        tuple_type m_its;

      private:
        template<int... M>
        using IS = typename std::integer_sequence<int, M...>;
        constexpr static auto _is() {
            return std::make_integer_sequence<int, sizeof...(IteratorTypes)>();
        }
        template<int... M>
        bool less_than(IS<M...>, zip_iterator &o) const {
            return (std::less(std::get<M>(m_its), std::get<M>(o.m_its)) || ...);
        }
        template<typename T>
        struct deref_type {
            using type = decltype(*std::declval<T>());
        };
        template<typename T>
        using deref_t = typename deref_type<T>::type;
        template<int... M>
        auto dereference(IS<M...>) {
            using ret_types =
              std::tuple<deref_t<decltype((std::get<M>(m_its)))>...>;
            using ret_type = std::tuple<
              choose_storage_t<std::tuple_element_t<M, ret_types>>...>;
            return ret_type{ *std::get<M>(m_its)... };
            // return
            // ret_type{std::forward<std::tuple_element_t<M,ret_types>>(*std::get<M>(m_its))...};
        }
        template<int... M>
        zip_iterator &increment(IS<M...>) {
            (++std::get<M>(m_its), ...);
            return *this;
        }
        template<int... M>
        bool equal_to(IS<M...>, const zip_iterator &o) const {
            return (std::equal_to<std::tuple_element_t<M, tuple_type>>()(
                      std::get<M>(m_its), std::get<M>(o.m_its))
                    || ...);
        }
    };

    template<typename... Types>
    struct zip_container {
        template<int... M>
        using IS = typename std::integer_sequence<int, M...>;
        constexpr static auto _is() {
            return std::make_integer_sequence<int, sizeof...(Types)>();
        }

        zip_container() = delete;
        zip_container(zip_container &&) = default;
        zip_container(const zip_container &) = delete;
        zip_container &operator=(zip_container &&) = delete;
        zip_container &operator=(const zip_container &) = delete;

        using iterator =
          zip_iterator<decltype(std::begin(std::declval<Types>()))...>;
        // using iterator = zip_iterator<typename
        // std::remove_reference_t<Types>::iterator...>;

        template<int... M>
        iterator _begin(IS<M...>) {
            return iterator(std::begin(std::get<M>(m_containers))...);
        }
        template<int... M>
        iterator _end(IS<M...>) {
            return iterator(std::end(std::get<M>(m_containers))...);
        }

        iterator begin() { return _begin(_is()); }

        iterator end() { return _end(_is()); }
        iterator begin() const { return _begin(_is()); }

        iterator end() const { return _end(_is()); }

        zip_container(Types &&... t) : m_containers{ std::forward<Types>(t)... } {}

      private:
        std::tuple<choose_storage_t<Types>...> m_containers;
    };
}// namespace detail

template<typename... Types>
auto zip(Types &&... t) {
    return detail::zip_container<Types...>(std::forward<Types>(t)...);
}

#endif
}// namespace mtao::iterator
