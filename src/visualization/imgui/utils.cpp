#include "mtao/visualization/imgui/utils.hpp"
namespace mtao::visualization::imgui::utils {

namespace internal {

    void strs_to_charPtr(const std::string *start, const std::string *end, const char **ptr_start) {
        std::transform(start, end, ptr_start, [](const std::string &str) -> const char * {
            return str.data();
        });
    }
}// namespace internal

// something to ease the process of preparing ordered collections of strings to something imgui can print in a Combo
std::vector<const char *> strs_to_charPtr(const std::vector<std::string> &strs) {
    std::vector<const char *> ret(strs.size());
    internal::strs_to_charPtr(strs.data(), strs.data() + strs.size(), ret.data());
    return ret;
}
}// namespace mtao::visualization::imgui::utils
