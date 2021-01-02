#include "mtao/logging/logger.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <spdlog/spdlog.h>

namespace mtao::logging {
    std::map<std::string,Logger> active_loggers;
    Logger* default_logger = &make_logger("default",Level::All);//Making this continue because multiple files might log to it over time 
    //Logger* default_logger = &make_file_logger("default","default.log",Level::All,true);//Making this continue because multiple files might log to it over time 

    const std::string level_strings[int(Level::All)+1] = {
        std::string("Off"),
        std::string("Fatal"),
        std::string("Error"),
        std::string("Warn"),
        std::string("Info"),
        std::string("Debug"),
        std::string("Trace"),
        std::string("All")
    };
    auto level_from_string(const std::string& str) -> Level {
        static auto make_map = []() {
            std::map<std::string,Level> mymap;
            for(int i = 0; i < static_cast<int>(Level::All)+1; ++i) {

                mymap[level_strings[i]] = static_cast<Level>(i);
            }
            return mymap;
        };
        static const std::map<std::string,Level> strmap = make_map();
        return strmap.at(str);

    }

    Logger::Logger(const std::string& alias, Level l): m_alias(alias), m_level(l) {
        write(Level::Info, "Beginning Log ", alias);
    }


    void Logger::add_file(const std::string& filename, Level level, bool continueFile) {

        if(auto it = m_outputs.find(filename);
                it == m_outputs.end()) {
            std::ofstream outstream = continueFile?std::ofstream(filename,std::ios::app):std::ofstream(filename);
            m_outputs[filename] = {std::move(outstream),level};
            auto continuing = [&]() -> std::string {
                if(continueFile) {
                    return "[append mode]";
                }                     return "";
               
            };
            write_cerr(Level::Info, "Adding log file ", filename, " to outputs of ", m_alias, " at level ", log_type_string(level), continuing());
            write_output(m_outputs[filename],Level::Info, "Logging to log ", m_alias, " at level ", log_type_string(level));
        }


    }
    auto make_logger(const std::string& alias, Level level) -> Logger& {

        if(auto it = active_loggers.find(alias);
                it != active_loggers.end()) {
            it->second.set_level(level);
            return it->second;
        }             auto&& out = active_loggers.emplace(alias,Logger(alias, level));
            if(active_loggers.empty()) {//TODO: remember why this was something to be worried about?
            }
            return std::get<0>(out)->second;

       
    }
    auto make_file_logger(const std::string& alias, const std::string& filename, Level level, bool continueFile) -> Logger& {

        Logger& logger = make_logger(alias,level);
        logger.add_file(filename,level, continueFile);
        return logger;

    }
    auto Logger::decorator(Level l, bool humanReadable) -> std::string {
        std::stringstream ss;
        ss << "[";
        if(humanReadable) {
            std::time_t res = std::time(nullptr);
            ss << "\033[30;1m";
            ss << std::put_time(std::localtime(&res), "%c");
            ss << "\033[0m";
        } else {
            std::time_t res = std::time(nullptr);
            //ss << current_time();
            ss << std::put_time(std::localtime(&res), "%c");
        }
        ss << "](" << log_type_string(l, humanReadable) << "): ";
        return ss.str();
    }

    void Logger::write_line(Level l, const std::string& str) {

        std::string dec = decorator(l);
        for(auto&& op: m_outputs) {
            write_line_nodec(op.second, l,dec + str);
        }
        write_line_cerr(l,str);
    }
    void Logger::write_line(Output& output, Level l,const std::string& str) {
        if(l <= output.level) {
            std::string dec = decorator(l);
            write_line_nodec(output, l,dec+str);
        }
    }
    void Logger::write_line_cerr(Level l,const std::string& str) {
        if(l <= m_level) {
            std::string dec = decorator(l,true);
            write_line_cerr_nodec(l,dec+str);
        }
    }

    void Logger::write_line_nodec(Output& output, Level l, const std::string& str) {
        if(l <= output.level) {
            output.out << str;
        }
    }
    void Logger::write_line_cerr_nodec(Level l, const std::string& str) {
        if(l <= m_level) {
            std::cerr << str;
        }
    }


    auto Logger::current_time() -> size_t {

        using ClockType = std::chrono::steady_clock;
        auto now = ClockType::now();
        return now.time_since_epoch().count();
    }
    auto Logger::log_type_string(Level l, bool color) -> const std::string& {
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
        }             return level_strings[int(l)];
       
    }

    Logger::Instance::~Instance() {
        m_ss << std::endl;
        logger->write_line(level,m_ss.str());
    }


    auto get_logger(const std::pair<std::string,Level>& pr) -> LoggerContext {
        return get_logger(pr.first,pr.second);
    }
    auto get_logger(const std::string& alias, Level level) -> LoggerContext {
        if(auto it = active_loggers.find(alias);
                it != active_loggers.end()) {
            return LoggerContext(&it->second,level);
        } 

            std::cerr << "Error: Logger " << alias << " not found! Making it" << std::endl;
            return LoggerContext(&make_logger(alias,level),level);
       
    }
    auto fatal() -> Logger::Instance{ return log(Level::Fatal); }
    auto error() -> Logger::Instance{ return log(Level::Error); }
    auto warn() -> Logger::Instance{ return log(Level::Warn); }
    auto info() -> Logger::Instance{ return log(Level::Info); }
    auto debug() -> Logger::Instance{ return log(Level::Debug); }
    auto trace() -> Logger::Instance{ return log(Level::Trace); }
    auto log(Level l) -> Logger::Instance{ return default_logger->instance(l); }


  // namespace logging
 // namespace logging
}  // namespace mtao // namespace mtao
