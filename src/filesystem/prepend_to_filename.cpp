
#include "mtao/filesystem/prepend_to_filename.hpp"

namespace mtao::filesystem {
std::filesystem::path prepend_to_filename(const std::filesystem::path &orig, const std::string &prefix) {
    auto parent = orig.parent_path();
    auto filename = orig.filename();
    return parent / (prefix + std::string(filename));
}
}// namespace mtao::filesystem
