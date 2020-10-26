#pragma once
#include <range/v3/view/indices.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/zip.hpp>

namespace mtao::iterator {


struct enumerate_fn  {
    template(typename... Rng) (
            requires viewable_range<Rng>...
            )
        auto operator()(Rng&&... rng) const {
            using FT = std::tuple_element<0,Rng...>
            using D = range_difference_t<FT>;
            using S = detail::iter_size_t<iterator_t<FT>>;
            return ranges::views::zip(ranges::views::detail::index_view<S,D>(), all(static_cast<Rng&&>(rng))...);


        }
};

template <typename... T>
auto enumerate(T&&... v) {

    /*
    return ranges::views::zip(ranges::views::indices,
                                 std::forward<T>(v)...);
    */
}

//template <typename... T>
//struct enumerate_: public ranges::views::zip {
//    enumerate_(T&&...): ranges::views::zip(ranges::views::indices,
//                                 std::forward<T>(v)...) {}
//};

template <typename... T>
auto enumerate_offset(int start, T&&... v) {
    return enumerate(std::forward<T>(v)...) | ranges::views::drop(start);
}
}  // namespace mtao::iterator
