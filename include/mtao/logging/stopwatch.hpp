#pragma once

#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <chrono>
#include <list>

namespace mtao::logging {


class HierarchicalStopwatch : std::enable_shared_from_this<HierarchicalStopwatch> {

  public:
    using Ptr = std::shared_ptr<HierarchicalStopwatch>;
    using WeakPtr = std::weak_ptr<HierarchicalStopwatch>;
    using clock = std::chrono::steady_clock;

    // choses one of several available loggers in this order:
    // logger passed in if not nullptr,
    // THEN logger in parent if parent is not nullptr,
    // default logger
    static Ptr create(std::string name, std::shared_ptr<spdlog::logger> logger = {}, spdlog::level::level_enum level = spdlog::level::info) {
        std::scoped_lock lock(s_parent_mutex);
        auto me = std::shared_ptr<HierarchicalStopwatch>(new HierarchicalStopwatch(name, logger, level));
        s_parent_stopwatch = me;
        return me;
    }

    ~HierarchicalStopwatch();

    const std::string &name() const;
    const std::string &hierarchical_name() const;
    std::list<std::string> hierarchy_names() const;
    std::list<size_t> parent_ids() const;
    std::list<size_t> hierarchy_ids() const;


  private:
    const std::string m_name;
    size_t m_id;
    Ptr m_parent = nullptr;
    const std::string m_hierarchical_name;
    std::shared_ptr<spdlog::logger> m_logger = s_default_logger;
    spdlog::level::level_enum m_level;

    std::chrono::time_point<clock> m_start_tp;

    //


  protected:
    HierarchicalStopwatch(std::string name, std::shared_ptr<spdlog::logger> logger = s_default_logger, spdlog::level::level_enum level = spdlog::level::info);
    static std::shared_ptr<spdlog::logger> s_default_logger;
    static WeakPtr s_parent_stopwatch;
    static std::mutex s_parent_mutex;
    static size_t s_current_log_index;
};

HierarchicalStopwatch::Ptr hierarchical_stopwatch(std::string name, std::shared_ptr<spdlog::logger> logger = {}, spdlog::level::level_enum level = spdlog::level::info);

}// namespace mtao::logging
