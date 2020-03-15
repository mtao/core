#include <iostream>
#include <mtao/linear_algebra/gram_schmidt.hpp>
#include <catch2/catch.hpp>
#include <iterator>
#include <iostream>
#include <algorithm>



template <typename Derived, typename Derived2>
void test_mats(const Eigen::MatrixBase<Derived>& A, const Eigen::MatrixBase<Derived2>& B) {
    REQUIRE(A.rows() == B.rows());
    REQUIRE(A.cols() == B.cols());
    if(A.rows() == B.rows() && A.cols() == B.cols()) {
        for(int i = 0; i < A.rows(); ++i) {
        for(int j = 0; j < B.cols(); ++j) {
            REQUIRE(A(i,j) == Approx(B(i,j)).margin(1e-5));
        }
        }
    }
}


TEST_CASE("GramSchmidt", "[arnoldi]") {

    for(auto&& D: {5,10,15}) {
        Eigen::MatrixXd L(D,D);
        L.setRandom();

        {
            //std::cout << L << std::endl;
            auto G = mtao::linear_algebra::gram_schmidt(L);
            //std::cout << G << std::endl;
            //std::cout << (G * G.transpose()) << std::endl;
            test_mats(G * G.transpose(),Eigen::MatrixXd::Identity(D,D));
        }
        {


            Eigen::MatrixXd P(D,3);
            P.setRandom();

            auto PG = mtao::linear_algebra::gram_schmidt(P);

            std::cout << PG.transpose() * PG << std::endl;

            L -= PG * PG.transpose() * L;

            auto G = mtao::linear_algebra::gram_schmidt(L);

            REQUIRE((PG.transpose() * L).norm() == Approx(0.0).margin(1e-5));
            REQUIRE((PG.transpose() * G).norm() == Approx(0.0).margin(1e-5));
            //std::cout << C.transpose() * L << std::endl;
            //std::cout << A.transpose() * G << std::endl;
            //std::cout << B.transpose() * G << std::endl;
            //std::cout << C.transpose() * G << std::endl;

            //REQUIRE((A.transpose() * L).norm() == Approx(0.0));
            //REQUIRE((B.transpose() * L).norm() == Approx(0.0));
            //REQUIRE((C.transpose() * L).norm() == Approx(0.0));
            //REQUIRE((A.transpose() * G).norm() == Approx(0.0));
            //REQUIRE((B.transpose() * G).norm() == Approx(0.0));
            //REQUIRE((C.transpose() * G).norm() == Approx(0.0));
        }
    }
}
