#include <iostream>
#include <mtao/python/load_python_function.hpp>

int main(int argc, char *argv[]) {


    std::string function_string =
      "def FUNC_NAME(p):\n"
      "  return np.array([2 * p,3*p])\n\n";

    mtao::python::PythonFunction func("def FUNC_NAME(x): return 5*x");
    std::cout << "Func made" << std::endl;

    std::cout << func(3).cast<double>() << std::endl;
}
