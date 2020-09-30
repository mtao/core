#pragma once
#include <mtao/types.hpp>
#include <iostream>
#include <mtao/type_utils.h>
#include "shell.hpp"
#include <range/v3/view/zip.hpp>

namespace mtao {
    namespace iterator {

        template <typename... Types>
            auto zip(Types&&... t) {
                return ranges::views::zip(std::forward<Types>(t)...);
                //return detail::zip_container<Types...>(std::forward<Types>(t)...);
            }
    }
}
