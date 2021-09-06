#include "mtao/filesystem/get_relative_path.hpp"

namespace mtao::filesystem {
std::filesystem::path get_relative_path(const std::filesystem::path &base_file, const std::string &new_path) {

    return base_file.parent_path() / new_path;
}
}// namespace mtao::filesystem

