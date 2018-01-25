#include "mtao/logging/profiler.hpp"
#include <iostream>

namespace mtao { namespace logging {
            std::map<std::pair<std::string,mtao::logging::Level>,std::map<std::string,std::pair<profiler::duration_type,int>>> profiler::s_durations;
            std::mutex profiler::s_mutex;

    profiler::~profiler() {
        auto d = duration();

        auto s = std::scoped_lock(s_mutex);
        write(d);

        if(auto it = s_durations.find(logger().info()); it != s_durations.end()) {
            auto& dm = it->second;
            if(auto it = dm.find(name()); it != dm.end()) {
                auto& val = it->second;
                val.first += d;
                val.second++;
            } else {
                dm[name()] = {d,1};
            }
        } else {
            s_durations[logger().info()][name()] = {d,1};
        }
    }
    void profiler::log_all() {
        auto s = std::scoped_lock(s_mutex);
        for(auto&& pr: s_durations) {
            auto&& lc = get_logger(pr.first);

            for(auto&& dp: pr.second) {
                auto dms = std::chrono::duration_cast<std::chrono::milliseconds>(dp.second.first);
                auto time = dms.count();
                int count  =dp.second.second;
                double avg = time / decltype(time)(count);
                lc.write( "profiler[" , dp.first, "] took a total: " , time , "ms in ",count," calls, average is ",avg, "ms");
            }

        }
    }
}}
