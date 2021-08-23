#pragma once
#include <vector>
#include <array>
#include <string>

namespace mtao::visualization::imgui::utils {

namespace internal {

    void strs_to_charPtr(const std::string *start, const std::string *end, const char **ptr_start);
}

std::vector<const char *> strs_to_charPtr(const std::vector<std::string> &strs);
template<std::size_t D>
std::array<const char *, D> strs_to_charPtr(const std::array<std::string, D> &strs) {
    std::array<const char *, D> ret;
    internal::strs_to_charPtr(strs.data(), strs.data() + D, ret.data());
    return ret;
}

}// namespace mtao::visualization::imgui::utils
