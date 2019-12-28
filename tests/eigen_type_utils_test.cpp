#include <mtao/eigen_utils.h>
#include <iostream>


int main(int argc, char * argv[]) 
{
    Eigen::MatrixXd a = Eigen::MatrixXd::Random(20,20);
    Eigen::ArrayXd b = Eigen::ArrayXd::Random(20,20);


    std::cout << mtao::eigen::finite(a) << std::endl;
    std::cout << std::endl;
    std::cout << mtao::eigen::finite(a*a) << std::endl;
    std::cout << std::endl;
    std::cout << mtao::eigen::finite(b) << std::endl;
}
