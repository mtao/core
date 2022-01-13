#include "mtao/logging/stopwatch.hpp"


namespace mtao::logging {
std::shared_ptr<spdlog::logger> HierarchicalStopwatch::s_default_logger = spdlog::default_logger();
std::mutex HierarchicalStopwatch::s_parent_mutex;
HierarchicalStopwatch::WeakPtr HierarchicalStopwatch::s_parent_stopwatch;


HierarchicalStopwatch::HierarchicalStopwatch(std::string name, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum level)
  //
  : m_name(std::move(name)),
    m_parent(s_parent_stopwatch.lock()),
    m_hierarchical_name(make_hierarchical_name()),
    m_logger(bool(logger) ? std::move(logger) : bool(m_parent) ? m_parent->m_logger
                                                               : s_default_logger),
    m_level(std::move(level)), m_start_tp(clock::now()) {

    m_logger->log(m_level, "{{\"type\":\"stopwatch_start\", \"name\":\"{}\"}}", hierarchical_name());
    //m_logger->log(m_level, "Stopwatch[{}] start", hierarchical_name());
}

HierarchicalStopwatch::~HierarchicalStopwatch() {
    auto end_tp = clock::now();
    std::chrono::duration<double, std::milli> dur(end_tp - m_start_tp);
    //m_logger->log(m_level, "Stopwatch[{}] end", hierarchical_name());
    // this duration might be off by a very small amount
    m_logger->log(m_level, "{{\"type\":\"stopwatch_end\", \"name\":\"{}\",\"duration\":{}}}", hierarchical_name(), dur.count());
    //m_logger->log(m_level, "Stopwatch[{}] took {}", hierarchical_name(), dur.count());


    std::scoped_lock lock(s_parent_mutex);
    s_parent_stopwatch = m_parent;
}

const std::string &HierarchicalStopwatch::name() const {
    return m_name;
}
const std::string &HierarchicalStopwatch::hierarchical_name() const {
    return m_hierarchical_name;
}

std::string HierarchicalStopwatch::make_hierarchical_name() const {
    if (bool(m_parent)) {
        return fmt::format("{}::{}", m_parent->hierarchical_name(), name());
    } else {
        return name();
    }
}
HierarchicalStopwatch::Ptr hierarchical_stopwatch(std::string name, std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum level) {
    return HierarchicalStopwatch::create(name, logger, level);
}

}// namespace mtao::logging
