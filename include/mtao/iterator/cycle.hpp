#pragma once
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/subrange.hpp>

namespace mtao {
    namespace iterator {

        template <typename Container>
            auto cycle(Container&& c) {
                return ranges::views::cycle(c);
            }

        template <typename BeginIt, typename EndIt>
            auto cycle(BeginIt&& b, EndIt&& e) {
                return ranges::make_subrange(b,e) | ranges::views::cycle;
            }

        template <typename BeginIt, typename EndIt>
            auto cycle_with_offset(BeginIt&& b, EndIt&& e, size_t offset) {
                return ranges::views::cycle(b,e) | ranges::views::drop(offset);
            }
        template <typename Container>
            auto cycle_with_offset(Container&& c, size_t offset) {
                return ranges::views::cycle(c) | ranges::views::drop(offset);
            }
    }


}
