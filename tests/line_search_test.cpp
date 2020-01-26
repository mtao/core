#include <iostream>
#include "mtao/optimization/line_search.hpp"

#include <Eigen/Dense>

struct QuadraticFunc {
    using Scalar = double;
    using Vector = Eigen::VectorXd;

    Scalar objective(const Vector& p) const {
        return p.transpose() * (A * p + b) + c;
    }
    Vector gradient(const Vector& p) const {
        Vector ret = 2 * A * p + b;
        return ret;
    }

    //Vector descent_direction(const Vector& p) const { return -gradient(p).normalized(); }
    Vector descent_direction(const Vector& p) const { 


        if(trinary) {
        return (-gradient(p)).unaryExpr([](double v) -> double {
                if(v > 0) { return 1; }
                else if(v < 0) { return -1; }
                else { return 0; }
                });
        } else {
            return -gradient(p).normalized();
        }

    }
    bool trinary = false;
    Eigen::MatrixXd A;
    Eigen::VectorXd b;
    Scalar c;

};



using namespace mtao::optimization;


int main(int argc, char * argv[]) {
    std::cout << "Eigen threads: " << Eigen::nbThreads() << std::endl;
    QuadraticFunc func;
    int N = 600;
    Eigen::MatrixXd A(N,N);
    Eigen::VectorXd b(N);


    if constexpr(true) {
        A.setIdentity();
        b.setOnes(N);


        func.A = A.transpose() * A;
        func.b = -2 * A.transpose() * b;
        func.c = b.dot(b);
    } else {

        A.setRandom();
        b.setRandom();
        func.A = A.transpose() * A;
        func.b = b;
        func.c = 5;
    }

    auto run = [&](auto&& opt) {
        opt.set_position(b);
        std::cout << "Optimal energy: " << opt.objective() << std::endl;
        std::cout << "Optimal gradient norm: " << opt.gradient().norm() << std::endl;
        opt.set_position(Eigen::VectorXd::Random(N));
        std::cout << "Initial energy: " << opt.objective() << std::endl;
        int iters = opt.run();
        std::cout << "Final energy: " << opt.objective() << " in " << iters << " iterations."<< std::endl;
        std::cout << std::endl;
        //std::cout << "position: " << opt.position().transpose() << std::endl;
    };
    auto run_run = [&]() {
    {
        std::cout << "Backtracking / armijo conditions" << std::endl;
        auto opt = make_backtracking_line_search(func);
        run(opt);
    }
    {
        std::cout << "weak wolfe conditions" << std::endl;
        auto opt = make_wolfe_line_search(func);
        opt.set_weak();
        run(opt);
    }
    {
        std::cout << "Strong wolfe conditions" << std::endl;
        auto opt = make_wolfe_line_search(func);
        opt.set_strong();
        run(opt);
    }
    };
    run_run();
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    func.trinary = true;
    std::cout << "Using a `trinary` which is lazy for something in the roughly right direction" << std::endl;
    run_run();
}
