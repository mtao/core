#pragma once

#include <chrono>
#include <mutex>
#include <fstream>
#include <map>
#include "mtao/logging/timer.hpp"

namespace mtao {
namespace logging {

    struct profiler : public timer {
      public:
        using timer::timer;

        static void log_all();
        ~profiler() override;
        static void clear();

        static const auto &durations() { return s_durations; }

      private:
        static std::map<std::pair<std::string, mtao::logging::Level>, std::map<std::string, std::pair<duration_type, int>>> s_durations;
        static std::mutex s_mutex;
    };
}// namespace logging
}// namespace mtao
