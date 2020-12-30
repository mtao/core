#pragma once
#include <mtao/type_utils.h>

#include <iostream>
#include <mtao/types.hpp>

#include "shell.hpp"

namespace mtao::iterator {
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
    struct lexicographical_iterator {
      public:
        using tuple_type = std::tuple<IteratorTypes...>;

        lexicographical_iterator(IteratorTypes &&... its)
          : m_its{ std::forward<IteratorTypes>(its)... } {}

        bool operator==(const lexicographical_iterator &o) const {
            return equal_to(_is(), o);
        }
        bool operator!=(const lexicographical_iterator &o) const {
            return !equal_to(_is(), o);
        }
        bool operator<(const lexicographical_iterator &o) const {
            return less_than(_is(), o);
        }

        lexicographical_iterator &operator++() { return increment(_is()); }

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
        bool less_than(IS<M...>, lexicographical_iterator &o) const {
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
            using ret_type =
              std::tuple<choose_storage_t<std::tuple_element_t<M, ret_types>>...>;
            return ret_type{ *std::get<M>(m_its)... };
            // return
            // ret_type{std::forward<std::tuple_element_t<M,ret_types>>(*std::get<M>(m_its))...};
        }
        template<int... M>
        lexicographical_iterator &increment(IS<M...>) {
            auto inc(auto &ita, auto &itb){};
            (inc(std::get<M>(m_its), std::get<M + 1>(m_its)), ...);
            return *this;
        }
        template<int... M>
        bool equal_to(IS<M...>, const lexicographical_iterator &o) const {
            return (std::equal_to<std::tuple_element_t<M, tuple_type>>()(
                      std::get<M>(m_its), std::get<M>(o.m_its))
                    || ...);
        }
    };

    template<typename... Types>
    struct lexicographical_container {
        template<int... M>
        using IS = typename std::integer_sequence<int, M...>;
        constexpr static auto _is() {
            return std::make_integer_sequence<int, sizeof...(Types)>();
        }

        lexicographical_container() = delete;
        lexicographical_container(lexicographical_container &&) = default;
        lexicographical_container(const lexicographical_container &) = delete;
        lexicographical_container &operator=(lexicographical_container &&) = delete;
        lexicographical_container &operator=(const lexicographical_container &) =
          delete;

        using iterator = lexicographical_iterator<decltype(
          std::begin(std::declval<Types>()))...>;
        // using iterator = lexicographical_iterator<typename
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

        lexicographical_container(Types &&... t)
          : m_containers{ std::forward<Types>(t)... } {}

      private:
        std::tuple<choose_storage_t<Types>...> m_containers;
    };
}// namespace detail

template<typename... Types>
auto lexicographical(Types &&... t) {
    return detail::lexicographical_container<Types...>(
      std::forward<Types>(t)...);
}
}// namespace mtao::iterator
