#pragma once
#include <filesystem>

namespace mtao::filesystem {


// given a file "/a/b/c/base_file" and a path "../d/e/file2" this function returns
// /a/b/c/../d/e/file2 if such a file exists
std::filesystem::path get_relative_path(const std::filesystem::path &base_file, const std::string &new_path);

}// namespace mtao::filesystem
