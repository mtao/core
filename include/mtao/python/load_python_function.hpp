#pragma once
#include <pybind11/embed.h>
//#include <stdatomic.h>
//#include <atomic>


namespace mtao::python {


//std::atomic<std::shared_ptr<pybind11::scoped_interpreter>> interpreter();
std::shared_ptr<pybind11::scoped_interpreter> interpreter();


class PythonFunction {
  public:
    static PythonFunction from_file(const std::string &filename);

    PythonFunction(const std::string &py_code = "", const std::vector<std::string> &libraries = { "numpy", "math" });


    void update_function(const std::string &py_code);
    void load_library(const std::string &library_name);

    // just forward data
    template<typename... Args>
    pybind11::object operator()(Args &&... args) const {
        return _function(std::forward<Args>(args)...);
    }

    const std::string &function_name() const { return _function_name; }

  private:
    std::string _function_name;
    pybind11::object _scope;
    pybind11::object _function;
    std::shared_ptr<pybind11::scoped_interpreter> _interpreter;
    std::string _implementation;
};


}// namespace mtao::python
