#pragma once

#include <spdlog/logger.h>
#include <filesystem>
namespace mtao::logging {

// use_sngle_json indicates whether or not the messages should be parsed as json or not
void set_json_format(spdlog::logger &logger, bool messages_are_json = false);

std::shared_ptr<spdlog::logger> make_json_file_logger(const std::string &name, const std::filesystem::path &path, bool messages_are_json = false);
}// namespace mtao::logging
