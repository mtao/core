#pragma once

#include <filesystem>

namespace mtao::filesystem {
std::filesystem::path prepend_to_filename(const std::filesystem::path &orig, const std::string &prefix);
}
