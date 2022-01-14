#include <mtao/logging/stopwatch.hpp>
#include <mtao/logging/json_sink.hpp>
#include <chrono>
#include <thread>

using namespace mtao::logging;

int main(int argc, char *argv[]) {
    using namespace std::chrono_literals;
    auto json_logger = make_json_file_logger("timing", "json_stopwatch.log", true);
    auto sw = hierarchical_stopwatch("root", json_logger);
    {
        auto sw = hierarchical_stopwatch("level1");
        spdlog::info("sleeping for 100ms");
        std::this_thread::sleep_for(100ms);
        {
            auto sw = hierarchical_stopwatch("level2a");
            spdlog::info("sleeping for 50ms");
            std::this_thread::sleep_for(50ms);
        }
        {
            auto sw = hierarchical_stopwatch("level2b");
            spdlog::info("sleeping for 50ms");
            std::this_thread::sleep_for(20ms);
        }
    }
}
