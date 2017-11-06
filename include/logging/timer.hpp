#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <fstream>
#include <map>
#include "logging/logger.hpp"

namespace mtao { namespace logging {

    struct timer {
        public:
            using ClockType = std::chrono::steady_clock;
            timer(const std::string& name = "Anon", bool print_start=false,const std::string& log_alias = "default", mtao::logging::Level l = mtao::logging::Level::Info);
            //timer(const std::string& name = "Anon", const std::string& out = filename): timer(name, std::ofstream(name)) {
            //}
            ~timer();

        private:
            std::string m_name;
            decltype(ClockType::now()) m_start;
            mtao::logging::LoggerContext m_logger;

    };
}}


#endif//TIMER_HPP

