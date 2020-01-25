#include <iostream>
#include "mtao/optimization/line_search.hpp"

#include <Eigen/Dense>

struct QuadraticFunc {
    using Scalar = double;
    using Vector = Eigen::VectorXd;

    Scalar objective(const Vector& p) const {
        return p.transpose() * A * p + b.dot(p) + c;
    }
    Vector gradient(const Vector& p) const {
        Vector ret = 2 * A * p + b;
        return ret;
    }

    Vector descent_direction(const Vector& p) const { return -gradient(p).normalized(); }
    Eigen::MatrixXd A;
    Eigen::VectorXd b;
    Scalar c;

};



using namespace mtao::optimization;
struct QuadraticBTLS: public QuadraticFunc, BacktrackingLineSearchBase<QuadraticBTLS> {
    using Opt = BacktrackingLineSearchBase<QuadraticBTLS>;
    using Scalar = QuadraticFunc::Scalar;
    using Vector = QuadraticFunc::Vector;
    using QuadraticFunc::objective;
    using QuadraticFunc::gradient;
    using QuadraticFunc::descent_direction;
    using QuadraticFunc::QuadraticFunc;
    using Opt::Base::set_position;
};
struct QuadraticWLS: public QuadraticFunc, WolfeLineSearchBase<QuadraticWLS> {
    using Opt = WolfeLineSearchBase<QuadraticWLS>;
    using Scalar = QuadraticFunc::Scalar;
    using Vector = QuadraticFunc::Vector;
    using QuadraticFunc::objective;
    using QuadraticFunc::gradient;
    using QuadraticFunc::descent_direction;
    using QuadraticFunc::QuadraticFunc;
    using Opt::Base::set_position;
};


int main(int argc, char * argv[]) {
    QuadraticWLS opt;
    opt.set_weak();
    //QuadraticBTLS opt;
    int N = 10;
    Eigen::MatrixXd A(N,N);
    Eigen::VectorXd b(N);
    A.setIdentity();
    b.setOnes(N);

    opt.A = A.transpose() * A;
    opt.b = -2 * A.transpose() * b;
    opt.QuadraticFunc::c = b.dot(b);
    //A.setRandom();
    //b.setRandom();
    //opt.b = b;
    //opt.c = 5;

    opt.set_position(b);
    std::cout << "Optimal energy: " << opt.Opt::Base::objective() << std::endl;
    std::cout << "Optimal gradient: " << opt.Opt::Base::gradient().transpose() << std::endl;
    opt.set_position(Eigen::VectorXd::Random(N));
    std::cout << "Initial energy: " << opt.Opt::Base::objective() << std::endl;
    opt.run();
    std::cout << "Final energy: " << opt.Opt::Base::objective() << std::endl;
}
