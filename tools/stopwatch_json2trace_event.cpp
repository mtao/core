#include <iostream>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

//{"time": "2022-01-13T19:42:37.112551-08:00", "name": "timing", "level": "info", "process": 20108, "thread": 20108, "message": {"type":"stopwatch_end", "name":"level1", "id": 1,,"duration":170.284814}}
//{"time": "2022-01-13T19:42:37.112552-08:00", "name": "timing", "level": "info", "process": 20108, "thread": 20108, "message": {"type":"stopwatch_end", "name":"root", "id": 0,,"duration":170.317494}}
//{"time": "2022-01-13T19:43:15.219814-08:00", "name": "timing", "level": "info", "process": 20210, "thread": 20210, "message": {"type":"stopwatch_start", "name":"root", "id": 0, "id_hierarchy": []}}
//{"time": "2022-01-13T19:43:15.219855-08:00", "name": "timing", "level": "info", "process": 20210, "thread": 20210, "message": {"type":"stopwatch_start", "name":"level1", "id": 1, "id_hierarchy": ["0"]}}
//{"time": "2022-01-13T19:43:15.320183-08:00", "name": "timing", "level": "info", "process": 20210, "thread": 20210, "message": {"type":"stopwatch_start", "name":"level2a", "id": 2, "id_hierarchy": ["0","1"]}}
//{"time": "2022-01-13T19:43:15.370346-08:00", "name": "timing", "level": "info", "process": 20210, "thread": 20210, "message": {"type":"stopwatch_end", "name":"level2a", "id": 2,,"duration":50.155502}}
//{"time": "2022-01-13T19:43:15.370354-08:00", "name": "timing", "level": "info", "process": 20210, "thread": 20210, "message": {"type":"stopwatch_start", "name":"level2b", "id": 3, "id_hierarchy": ["0","1"]}}
//{"time": "2022-01-13T19:43:15.390437-08:00", "name": "timing", "level": "info", "process": 20210, "thread": 20210, "message": {"type":"stopwatch_end", "name":"level2b", "id": 3,,"duration":20.081907}}
//{"time": "2022-01-13T19:43:15.390441-08:00", "name": "timing", "level": "info", "process": 20210, "thread": 20210, "message": {"type":"stopwatch_end", "name":"level1", "id": 1,,"duration":170.586634}}
//{"time": "2022-01-13T19:43:15.390442-08:00", "name": "timing", "level": "info", "process": 20210, "thread": 20210, "message": {"type":"stopwatch_end", "name":"root", "id": 0,,"duration":170.630974}}


int main(int argc, char *argv[]) {
    nlohmann::json outjs = nlohmann::json::array();
    std::map<size_t, std::string> id_name_map;
    using namespace std::chrono_literals;
    try {

        nlohmann::json injs;
        while (std::cin >> injs) {
            //std::cout << injs << std::endl;
            nlohmann::json ejs;
            size_t id = injs["message"]["id"];
            size_t esecs_int = injs["epoch_secs"];
            int ems_int = injs["epoch_ms"];
            std::chrono::milliseconds epoch = std::chrono::seconds(esecs_int) + std::chrono::milliseconds(ems_int);

            //spdlog::info("{}ms", epoch.count());
            if (injs["message"]["type"] == "stopwatch_start") {
                std::string name = injs["message"]["name"];
                id_name_map[id] = name;
                ejs["ph"] = "B";
            } else if (injs["message"]["type"] == "stopwatch_end") {
                ejs["ph"] = "E";
            }

            ejs["name"] = id_name_map.at(id);
            ejs["cat"] = injs["level"];
            ejs["ts"] = std::chrono::microseconds(epoch).count();
            //ejs["tts"] = "";
            ejs["pid"] = injs["process"];
            ejs["tid"] = injs["thread"];
            //ejs["args"]="";
            outjs.emplace_back(ejs);
        }
    } catch (const nlohmann::detail::parse_error &e) {
        std::cout << outjs << std::endl;
        return 0;
    }
    std::cout << outjs << std::endl;
    return 0;
}
