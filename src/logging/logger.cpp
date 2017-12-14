#include "logging/logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace mtao { namespace logging {
    std::map<std::string,Logger> active_loggers;
    Logger* default_logger = &make_logger("default","default.log",Level::All,true);//Making this continue because multiple files might log to it over time 

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
    Level level_from_string(const std::string& str) {
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
            std::ofstream outstream(filename);
            if(continueFile) {
                outstream.seekp(0, std::ios_base::end);
            }
            m_outputs[filename] = {std::move(outstream),level};
            auto continuing = [&]() -> std::string {
                if(continueFile) {
                    return "[append mode]";
                } else {
                    return "";
                }
            };
            write_cout(Level::Info, "Adding log file ", filename, " to outputs of ", m_alias, " at level ", log_type_string(level), continuing());
            write_output(m_outputs[filename],Level::Info, "Logging from log ", m_alias, " at level ", log_type_string(level));
        }


    }
    Logger& make_logger(const std::string& alias, Level level) {

        if(auto it = active_loggers.find(alias);
                it != active_loggers.end()) {
            it->second.set_level(level);
            return it->second;
        } else {
            auto&& out = active_loggers[alias] = Logger(alias, level);
            if(active_loggers.empty()) {//TODO: remember why this was something to be worried about?
            }
            return out;

        }
    }
    Logger& make_logger(const std::string& alias, const std::string& filename, Level level, bool continueFile) {

        Logger& logger = make_logger(alias,level);
        logger.add_file(filename,level, continueFile);
        return logger;

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

        std::string dec = decorator(l);
        for(auto&& op: m_outputs) {
            write_line_nodec(op.second, l,dec + str);
        }
        write_line_cout(l,str);
    }
    void Logger::write_line(Output& output, Level l,const std::string& str) {
        if(l <= output.level) {
            std::string dec = decorator(l);
            write_line_nodec(output, l,dec);
        }
    }
    void Logger::write_line_cout(Level l,const std::string& str) {
        if(l <= m_level) {
            std::string dec = decorator(l,true);
            write_line_cout_nodec(l,dec+str);
        }
    }

    void Logger::write_line_nodec(Output& output, Level l, const std::string& str) {
        if(l <= output.level) {
            output.out << str;
        }
    }
    void Logger::write_line_cout_nodec(Level l, const std::string& str) {
        if(l <= m_level) {
            std::cout << str;
        }
    }


    size_t Logger::current_time() const {

        using ClockType = std::chrono::steady_clock;
        auto now = ClockType::now();
        return now.time_since_epoch().count();
    }
    const std::string& Logger::log_type_string(Level l, bool color) {
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
            return level_strings[int(l)];
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
    Logger::Instance fatal(){ return log(Level::Fatal); }
    Logger::Instance error(){ return log(Level::Error); }
    Logger::Instance warn(){ return log(Level::Warn); }
    Logger::Instance info(){ return log(Level::Info); }
    Logger::Instance debug(){ return log(Level::Debug); }
    Logger::Instance trace(){ return log(Level::Trace); }
    Logger::Instance log(Level l){ return default_logger->instance(l); }


}}
