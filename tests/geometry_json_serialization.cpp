#include <iostream>
#include <mtao/json/bounding_box.hpp>
#include <mtao/json/vector.hpp>


int main(int argc, char *argv[]) {
    Eigen::AlignedBox<double, 2> bbox;
    bbox.min().setZero();
    bbox.max().setOnes();

    nlohmann::json js;
    js["bounding_box"] = bbox;


    auto bb = js["bounding_box"].get<Eigen::AlignedBox<double, 2>>();
    std::cout << bb.min().transpose() << std::endl;
    std::cout << bb.max().transpose() << std::endl;


    js["Normal"] = Eigen::Vector3d::UnitZ();
    Eigen::Vector3d N = js["Normal"].get<Eigen::Vector3d>();
    std::cout << "N: " << N.transpose() << std::endl;


    js["bbox2"] = mtao::json::bounding_box2json(bbox);

    std::cout << js.dump(2) << std::endl;
    //std::cout << js["Normal"].get<Eigen::Vector3d>();
    //
    //
    {
        std::cout << "Doing bbox2" << std::endl;
        auto bb = js["bbox2"].get<Eigen::AlignedBox<double, 2>>();
        std::cout << bb.min().transpose() << std::endl;
        std::cout << bb.max().transpose() << std::endl;
    }


    auto vec = mtao::json::json2vector<double, 3>(js["Normal"]);
    std::cout << vec.transpose() << std::endl;


    Eigen::Matrix3d A = Eigen::Matrix3d::Random();
    std::cout << "Random matrix: \n"
              << A << std::endl;
    js["A"] = A;
    std::cout << "Jsoned:\n"
              << js["A"].dump(2) << std::endl;

    Eigen::MatrixXf B = js["A"].get<Eigen::MatrixXf>();
    std::cout << "Dynamic float sized A:\n"
              << B << std::endl;
}
