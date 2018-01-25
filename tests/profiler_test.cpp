#include <iostream>
#include <stdlib.h>
#include "mtao/logging/timer.hpp"
#include "mtao/logging/profiler.hpp"


int main() {
    std::cout << getenv("_") << std::endl;
    mtao::logging::make_logger("A", "a.log", mtao::logging::Level::All);

    auto log = mtao::logging::get_logger("A",mtao::logging::Level::Fatal);
    log.write("welp",3,4);

    for(int i = 0; i < 5; ++i) 
    {
        auto profiler = mtao::logging::profiler("task",false,"A");
        int x = 0;
        for(int i = 0; i < (1 << 10); ++i) {
            mtao::logging::info() <<"iteration: " << i;
            mtao::logging::fatal() <<"iteration: " << i;
            mtao::logging::error() <<"iteration: " << i;
            mtao::logging::warn() <<"iteration: " << i;
            mtao::logging::info() <<"iteration: " << i;
            mtao::logging::debug() <<"iteration: " << i;
            mtao::logging::trace() <<"iteration: " << i;
            x += i;        
        }
    }
    mtao::logging::profiler::log_all();

}
