#include "mtao/python/load_python_function.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <string>
#include <mutex>
#include <thread>
#include <random>
#include <algorithm>

namespace mtao::python {
namespace {

    std::string get_new_random_name(size_t size = 8) {

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist('a', 'z');

        auto interp = interpreter();
        pybind11::object scope = pybind11::module_::import("__main__").attr("__dict__");

        pybind11::object already_exists_predicate = scope.attr("__contains__");

        // infinite loop... famous last words "it'll never happen"
        while (true) {
            std::string ret;
            std::generate_n(std::back_inserter(ret), size, [&] { return dist(mt); });
            auto res = already_exists_predicate(ret);
            if (!res.cast<bool>()) {
                return ret;
            }
        }
    }
}// namespace

std::weak_ptr<pybind11::scoped_interpreter> _interpreter;
//std::atomic<std::weak_ptr<pybind11::scoped_interpreter>> _interpreter;
std::mutex update_interpreter_mutex;


std::shared_ptr<pybind11::scoped_interpreter> interpreter() {
    //std::atomic<std::weak_ptr<pybind11::scoped_interpreter>> interpreter() {


    std::scoped_lock lock(update_interpreter_mutex);
    if (_interpreter.expired()) {
        std::shared_ptr<pybind11::scoped_interpreter> ret(std::make_shared<pybind11::scoped_interpreter>());
        _interpreter = ret;
        return ret;
    } else {

        return _interpreter.lock();
    }
}


PythonFunction::PythonFunction(const std::string &py_code, const std::vector<std::string> &libraries) : _function_name(get_new_random_name()), _interpreter(interpreter()) {
    _scope = pybind11::module_::import("__main__").attr("__dict__");
    for (auto &&lib : libraries) {
        load_library(lib);
    }

    update_function(py_code);
}


bool PythonFunction::update_function(const std::string &py_code) {
    try {
        if (py_code.empty()) {
            pybind11::exec(fmt::format("def {}(p): return p\n\n", function_name()));
        } else {

            std::string str(py_code);
            for (size_t idx = 0; idx != std::string::npos;) {
                idx = str.find("FUNC_NAME", idx);
                if (idx == std::string::npos) {
                    break;
                }
                str.replace(idx, 9, function_name());
            }
            pybind11::exec(str);
        }
        // some extra newlines to make sure that the user exited things properly
        pybind11::exec("\n\n");

        _function = pybind11::module_::import("__main__").attr(function_name().c_str());
        return true;
    } catch (std::exception &e) {
        spdlog::warn("Failed to update function due to an error. Clearing held function:\n {}", e.what());
        _function = {};
    }
    return false;
}
bool PythonFunction::load_library(const std::string &library_name) {
    try {
        pybind11::exec(fmt::format("import {}\n", library_name));
        return true;
    } catch (std::exception &e) {
        spdlog::warn("Failed to load a python library:\n {}", e.what());
        return false;
    }
}

}// namespace mtao::python
