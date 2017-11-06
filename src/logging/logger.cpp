#include "logging/logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace mtao { namespace logging {
    std::map<std::string,mtao::logging::Logger> active_loggers;
    Logger* default_logger = &make_logger();

    Logger::Logger(const std::string& alias, const std::string& filename, Level l): m_filename(filename), m_alias(alias), m_outstream(filename), m_level(l), m_cout_level(l) {
        m_outstream.seekp(0, std::ios_base::end);
        write(Level::Info, "Beginning Log ", alias);
    }
    Logger& make_logger(const std::string& alias, const std::string& filename, Level level) {

        if(auto it = active_loggers.find(alias);
                it != active_loggers.end()) {
            return it->second;
        } else {
            auto&& out = active_loggers[alias] = std::move(Logger(alias,filename, level));
            if(active_loggers.empty()) {
            }
            return out;

        }
    }
    std::string Logger::decorator(Level l, bool humanReadable) const {
        std::stringstream ss;
        ss << "[";
        if(humanReadable) {
            std::time_t res = std::time(NULL);
            ss << "\033[30;1m";
            ss << std::put_time(std::localtime(&res), "%c");
            ss << "\033[0m";
        } else {
            ss << current_time();
        }
        ss << "](" << log_type_string(l, humanReadable) << "): ";
        return ss.str();
    }

    void Logger::write_line(Level l, const std::string& str) {

        if(l <= m_level) {
            std::string dec = decorator(l);
            m_outstream << dec << str;
        }
        if( l <= m_cout_level ) {
            std::string dec = decorator(l,true);
            std::cout << dec << str;
        }
    }


    size_t Logger::current_time() const {

        using ClockType = std::chrono::steady_clock;
        auto now = ClockType::now();
        return now.time_since_epoch().count();
    }
    const std::string& Logger::log_type_string(Level l, bool color) {
        const static std::string strs[int(Level::All)+1] = {
            std::string("Off"),
            std::string("Fatal"),
            std::string("Error"),
            std::string("Warn"),
            std::string("Info"),
            std::string("Debug"),
            std::string("Trace"),
            std::string("All")
        };
        const static std::string col_strs[int(Level::All)+1] = {
            std::string("\033[30mOff\033[0m"),
            std::string("\033[31;1;7mFatal\033[0m"),
            std::string("\033[31;1mError\033[0m"),
            std::string("\033[33mWarn\033[0m"),
            std::string("\033[32;7;1mInfo\033[0m"),
            std::string("\033[32;1mDebug\033[0m"),
            std::string("\033[0mTrace\033[0m"),
            std::string("\033[30mAll\033[0m")
        };
        if(color) {
            return col_strs[int(l)];
        } else {
            return strs[int(l)];
        }
    }

    Logger::Instance::~Instance() {
        m_ss << std::endl;
        logger->write_line(level,m_ss.str());
    }


    LoggerContext get_logger(const std::string& alias, Level level) {
        if(auto it = active_loggers.find(alias);
                it != active_loggers.end()) {
            return LoggerContext(&it->second,level);
        } else {
            return LoggerContext();
            std::cerr << "Error: Logger " << alias << " not found!" << std::endl;
        }
    }
    Logger::Instance fatal(){ return default_logger->instance(Level::Fatal); }
    Logger::Instance error(){ return default_logger->instance(Level::Error); }
    Logger::Instance warn(){ return default_logger->instance(Level::Warn); }
        Logger::Instance info(){ return default_logger->instance(Level::Info); }
        Logger::Instance debug(){ return default_logger->instance(Level::Debug); }
        Logger::Instance trace(){ return default_logger->instance(Level::Trace); }


}}
