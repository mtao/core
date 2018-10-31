#pragma once
#include <tuple>
#include <mtao/types.h>
#include <iostream>
#include <iostream>

namespace mtao {
    namespace iterator {
        namespace detail {
            template <typename T, bool Expr =std::is_lvalue_reference_v<T>>
                struct zip_choose_reference {
                    using value_type = T&;
                };
            template <typename T>
                struct zip_choose_reference<T,false> {
                    using value_type = T;
                };
            template <typename T>
                struct zip_choose_reference<const T,false> {
                    using value_type = const T;
                };
            template <typename T>
                struct zip_choose_reference<const T,true> {
                    using value_type = const T&;
                };

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

            template <typename T, bool Expr = is_initializer_list_v<remove_cvref_t<T>>>
                struct filter_initializer_list {
                    using  value_type = std::vector<typename T::value_type>;
                };
            template <typename T>
                struct filter_initializer_list<T,false> {
                    using value_type = T;
                };
            template <typename T>
                using filter_initializer_list_t = typename filter_initializer_list<T>::value_type;

            template <typename T>
                using zip_choose_reference_t = typename zip_choose_reference<T>::value_type;

            template <typename... IteratorTypes>
                struct zip_iterator {
                    public:

                        using tuple_type = std::tuple<IteratorTypes...>;


                        zip_iterator(IteratorTypes&&... its): m_its{std::forward<IteratorTypes>(its)...} {}

                        bool operator==(const zip_iterator& o) const {return equal_to(_is(),o);}
                        bool operator!=(const zip_iterator& o) const {return !equal_to(_is(),o);}
                        bool operator<(const zip_iterator& o) const{return less_than(_is(),o);}

                        zip_iterator& operator++() {return increment(_is());}

                        auto operator*() {return dereference(_is());}

                    private:
                        tuple_type m_its;

                    private:
                        template <int... M>
                            using IS = typename std::integer_sequence<int,M...>;
                        constexpr static auto _is() { return std::make_integer_sequence<int,sizeof...(IteratorTypes)>(); }
                        template <int... M>
                            bool less_than(IS<M...>, zip_iterator& o) const {
                                return (std::less(std::get<M>(m_its),std::get<M>(o.m_its))  || ... );
                            }
                        template <int... M>
                            auto dereference(IS<M...>) {
                                using ret_types = std::tuple<decltype(std::get<M>(m_its).operator*())...>;
                                using ret_type = std::tuple<zip_choose_reference_t<std::tuple_element_t<M,ret_types>>...>;
                                return ret_type{std::forward<std::tuple_element_t<M,ret_types>>(std::get<M>(m_its).operator*())...};
                            }
                        template <int... M>
                            zip_iterator& increment(IS<M...>) {
                                (++std::get<M>(m_its),...);
                                return *this;
                            }
                        template <int... M>
                            bool equal_to(IS<M...>,const zip_iterator& o) const{
                                return (std::equal_to<std::tuple_element_t<M,tuple_type>>()(std::get<M>(m_its),std::get<M>(o.m_its))  || ... );
                            }
                };

            template <typename... Types>
                struct zip_container {


                    template <int... M>
                        using IS = typename std::integer_sequence<int,M...>;
                    constexpr static auto _is() { return std::make_integer_sequence<int,sizeof...(Types)>(); }

                    zip_container() = delete;
                    zip_container(zip_container&&) = default;
                    zip_container(const zip_container&) = delete;
                    zip_container& operator=(zip_container&&) = delete;
                    zip_container& operator=(const zip_container&) = delete;

                    using iterator = zip_iterator<typename std::remove_reference_t<Types>::iterator...>;

                    template <int... M>
                        iterator _begin(IS<M...>) const { return iterator(std::get<M>(m_containers).begin() ...); }
                    template <int... M>
                        iterator _end(IS<M...>) const { return iterator(std::get<M>(m_containers).end() ...); }

                    iterator begin() const { return _begin(_is());}


                    iterator end() const { return _end(_is());}



                    zip_container(Types&&... t): m_containers{std::forward<Types>(t)...} {
                    }
                    /*
                       zip_container(Types&&... t)
                       : m_begin{t.begin()...}
                       , m_end{t.end()...}
                       {}
                       auto begin() const { return m_begin; }
                       auto end() const { return m_end; }
                       */


                    private:

                    /*
                       iterator m_begin;
                       iterator m_end;
                       */
                    std::tuple<zip_choose_reference_t<Types>...> m_containers;
                };
        }

        template <typename... Types>
            auto zip(Types&&... t) {
                return detail::zip_container<Types...>(std::forward<Types>(t)...);
            }
    }
}
