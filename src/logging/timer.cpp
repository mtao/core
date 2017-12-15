#include "mtao/logging/timer.hpp"
#include <iostream>
#include <sstream>

namespace mtao { namespace logging {
    timer::timer(const std::string& name, bool print_start,const std::string& log_alias, mtao::logging::Level level): m_name(name), m_start(ClockType::now()), m_logger(mtao::logging::get_logger(log_alias, level)) {

        if(print_start) {
            m_logger.write( "timer[", m_name, "] start");
        }
    }


    timer::~timer() {
        auto end = ClockType::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start);
        std::stringstream ss;
        m_logger.write( "timer[" , m_name , "] took: " , duration.count() , "ms");
    }
}}
