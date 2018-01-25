#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <mutex>
#include <fstream>
#include <map>
#include "mtao/logging/logger.hpp"

namespace mtao { namespace logging {

    struct timer {
        public:
            using clock_type = std::chrono::steady_clock;
            using duration_type = std::chrono::duration<double>;
            timer(const std::string& name = "Anon", bool print_start=false,const std::string& log_alias = "default", mtao::logging::Level l = mtao::logging::Level::Trace);
            //timer(const std::string& name = "Anon", const std::string& out = filename): timer(name, std::ofstream(name)) {
            //}
            std::chrono::duration<double> duration() const;
            void write(const std::chrono::duration<double>& dur);
            virtual ~timer();
            const std::string& name() const { return m_name; }
            const mtao::logging::LoggerContext& logger() const { return m_logger; }

        private:
            std::string m_name;
            decltype(clock_type::now()) m_start;
            mtao::logging::LoggerContext m_logger;

    };
}}


#endif//TIMER_HPP

