#include "mtao/logging/timer.hpp"
#include <iostream>

namespace mtao {
namespace logging {

    timer::timer(const std::string &name, bool print_start, const std::string &log_alias, mtao::logging::Level level) : m_name(name), m_start(clock_type::now()), m_logger(mtao::logging::get_logger(log_alias, level)) {

        if (print_start) {
            m_logger.write("timer[", m_name, "] start");
        }
    }

    std::chrono::duration<double> timer::duration() const {
        auto end = clock_type::now();

        return end - m_start;
    }

    void timer::write(const std::chrono::duration<double> &dur) {
        auto dms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
        m_logger.write("timer[", m_name, "] took: ", dms.count(), "ms");
    }

    timer::~timer() {
        write(duration());
    }

}// namespace logging
}// namespace mtao
