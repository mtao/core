#include <filesystem>
#include "mtao/filesystem/get_relative_path.hpp"
namespace mtao::utils {
std::filesystem::path get_relative_path(const std::filesystem::path &base_file, const std::string &new_path) {
    return filesystem::get_relative_path(base_file, new_path);
}
}// namespace mtao::utils
